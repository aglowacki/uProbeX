/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

/**
 * Advanced unit tests for BlueskyComm with ZMQ mocking.
 *
 * This file demonstrates how to test BlueskyComm with a mock ZMQ server.
 * Note: This requires creating a testable version of BlueskyComm that
 * allows dependency injection of the ZMQ socket.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <thread>
#include <chrono>
#include <zmq.hpp>
#include "mvc/BlueskyPlan.h"

using ::testing::_;
using ::testing::Return;

//---------------------------------------------------------------------------
// Mock ZMQ Server for Testing
//---------------------------------------------------------------------------

class MockBlueskyServer {
private:
    zmq::context_t context_;
    zmq::socket_t socket_;
    std::thread server_thread_;
    bool running_;
    std::string bind_address_;

public:
    MockBlueskyServer(const std::string& address = "tcp://127.0.0.1:60615")
        : context_(1),
          socket_(context_, ZMQ_REP),
          running_(false),
          bind_address_(address) {
    }

    ~MockBlueskyServer() {
        stop();
    }

    void start() {
        socket_.bind(bind_address_);
        running_ = true;
        server_thread_ = std::thread([this]() {
            this->run();
        });
    }

    void stop() {
        running_ = false;
        if (server_thread_.joinable()) {
            server_thread_.join();
        }
    }

    void run() {
        while (running_) {
            zmq::message_t request;

            // Set timeout for recv to allow checking running_ flag
            socket_.set(zmq::sockopt::rcvtimeo, 100);

            auto result = socket_.recv(request, zmq::recv_flags::none);
            if (!result) {
                continue; // Timeout or error, check running_ flag
            }

            // Parse request
            QJsonDocument req_doc = QJsonDocument::fromJson(
                QByteArray::fromRawData((char*)request.data(), request.size())
            );
            QJsonObject req_obj = req_doc.object();

            // Generate response based on method
            QJsonObject response = handleRequest(req_obj);

            // Send response
            QByteArray resp_data = QJsonDocument(response).toJson();
            zmq::message_t reply(resp_data.data(), resp_data.length());
            socket_.send(reply, zmq::send_flags::none);
        }
    }

private:
    QJsonObject handleRequest(const QJsonObject& request) {
        QString method = request["method"].toString();
        QJsonObject response;

        if (method == "environment_open") {
            response["success"] = true;
            response["msg"] = "Environment opened successfully";
        }
        else if (method == "environment_close") {
            response["success"] = true;
            response["msg"] = "Environment closed successfully";
        }
        else if (method == "queue_start") {
            response["success"] = true;
            response["msg"] = "Queue started";
        }
        else if (method == "queue_stop") {
            response["success"] = true;
            response["msg"] = "Queue stopped";
        }
        else if (method == "queue_item_add") {
            response["success"] = true;
            response["msg"] = "Plan queued successfully";
            QJsonObject item = request["params"].toObject()["item"].toObject();
            item["item_uid"] = "generated-uuid-12345";
            response["item"] = item;
        }
        else if (method == "queue_item_update") {
            response["success"] = true;
            response["msg"] = "Plan updated successfully";
            response["item"] = request["params"].toObject()["item"];
        }
        else if (method == "queue_item_move") {
            response["success"] = true;
            response["msg"] = "Plan moved successfully";
        }
        else if (method == "queue_item_remove") {
            response["success"] = true;
            response["msg"] = "Plan removed successfully";
            QJsonObject item;
            item["item_uid"] = request["params"].toObject()["uid"].toString();
            response["item"] = item;
        }
        else if (method == "queue_get") {
            response["success"] = true;
            response["msg"] = "";

            // Create mock queued items
            QJsonArray items;
            QJsonObject item1;
            item1["name"] = "count";
            item1["item_uid"] = "queue-item-1";
            QJsonObject meta1;
            meta1["name"] = "Count Scan 1";
            item1["meta"] = meta1;
            QJsonObject kwargs1;
            kwargs1["num"] = 10;
            item1["kwargs"] = kwargs1;
            items.append(item1);

            response["items"] = items;

            // Create mock running item
            QJsonObject running;
            running["name"] = "scan";
            running["item_uid"] = "running-item-1";
            QJsonObject props;
            props["time_start"] = 1234567890.0;
            running["properties"] = props;
            response["running_item"] = running;
        }
        else if (method == "plans_allowed") {
            response["success"] = true;
            response["msg"] = "";

            QJsonObject plans;

            // Mock count plan
            QJsonObject countPlan;
            countPlan["name"] = "count";
            countPlan["description"] = "Count detectors";
            countPlan["module"] = "bluesky.plans";

            QJsonArray params;
            QJsonObject param1;
            param1["name"] = "num";
            param1["default"] = 1;
            param1["description"] = "int: Number of counts";
            params.append(param1);

            countPlan["parameters"] = params;
            plans["count"] = countPlan;

            response["plans_allowed"] = plans;
        }
        else if (method == "history_get") {
            response["success"] = true;
            response["msg"] = "";

            QJsonArray items;
            QJsonObject item1;
            item1["name"] = "count";
            item1["item_uid"] = "finished-item-1";
            QJsonObject meta1;
            meta1["name"] = "Completed Scan";
            item1["meta"] = meta1;

            QJsonObject result;
            result["exit_status"] = "completed";
            result["time_start"] = 1234567890.0;
            result["time_stop"] = 1234567900.0;
            result["msg"] = "";
            result["traceback"] = "";
            QJsonArray run_uids;
            run_uids.append("run-uid-123");
            result["run_uids"] = run_uids;

            item1["result"] = result;
            items.append(item1);

            response["items"] = items;
        }
        else if (method == "history_clear") {
            response["success"] = true;
            response["msg"] = "History cleared";
        }
        else {
            response["success"] = false;
            response["msg"] = "Unknown method: " + method;
        }

        return response;
    }
};

//---------------------------------------------------------------------------
// Test Fixture with Mock Server
//---------------------------------------------------------------------------

class BlueskyCommMockTest : public ::testing::Test {
protected:
    MockBlueskyServer* server;
    zmq::context_t* context;
    QString test_ip;

    void SetUp() override {
        server = new MockBlueskyServer("tcp://127.0.0.1:60616");
        server->start();

        // Give server time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        context = new zmq::context_t(1);
        test_ip = "127.0.0.1:60616";
    }

    void TearDown() override {
        server->stop();
        delete server;
        delete context;
    }
};

//---------------------------------------------------------------------------
// Integration Tests with Mock Server
//---------------------------------------------------------------------------

/*
// Uncomment these tests when BlueskyComm is refactored to allow
// custom port configuration (currently hardcoded to 60615)

TEST_F(BlueskyCommMockTest, OpenEnvironment) {
    BlueskyComm comm(context, "127.0.0.1:60616");
    QString msg;

    bool result = comm.open_env(msg);

    EXPECT_TRUE(result);
    EXPECT_EQ(msg, "Environment opened successfully");
}

TEST_F(BlueskyCommMockTest, CloseEnvironment) {
    BlueskyComm comm(context, "127.0.0.1:60616");
    QString msg;

    bool result = comm.close_env(msg);

    EXPECT_TRUE(result);
    EXPECT_EQ(msg, "Environment closed successfully");
}

TEST_F(BlueskyCommMockTest, StartQueue) {
    BlueskyComm comm(context, "127.0.0.1:60616");
    QString msg;

    bool result = comm.start_queue(msg);

    EXPECT_TRUE(result);
    EXPECT_EQ(msg, "Queue started");
}

TEST_F(BlueskyCommMockTest, QueuePlan) {
    BlueskyComm comm(context, "127.0.0.1:60616");
    QString msg;

    BlueskyPlan plan;
    plan.name = "Test Scan";
    plan.type = "count";

    BlueskyParam param;
    param.name = "num";
    param.default_val = "10";
    param.kind = BlueskyParamType::Double;
    plan.parameters.push_back(param);

    bool result = comm.queue_plan(msg, plan);

    EXPECT_TRUE(result);
    EXPECT_FALSE(plan.uuid.isEmpty());
    EXPECT_EQ(plan.uuid, "generated-uuid-12345");
}

TEST_F(BlueskyCommMockTest, GetAvailableScans) {
    BlueskyComm comm(context, "127.0.0.1:60616");
    QString msg;
    std::map<QString, BlueskyPlan> plans;

    bool result = comm.get_avail_scans(plans, msg);

    EXPECT_TRUE(result);
    EXPECT_GT(plans.size(), 0);
    EXPECT_TRUE(plans.find("count") != plans.end());

    BlueskyPlan& countPlan = plans["count"];
    EXPECT_EQ(countPlan.name, "count");
    EXPECT_EQ(countPlan.description, "Count detectors");
    EXPECT_GT(countPlan.parameters.size(), 0);
}

TEST_F(BlueskyCommMockTest, GetQueuedScans) {
    BlueskyComm comm(context, "127.0.0.1:60616");
    QString msg;
    std::vector<BlueskyPlan> queued_plans;
    BlueskyPlan running_plan;

    bool result = comm.get_queued_scans(msg, queued_plans, running_plan);

    EXPECT_TRUE(result);
    EXPECT_GT(queued_plans.size(), 0);
    EXPECT_EQ(running_plan.type, "scan");
    EXPECT_GT(running_plan.result.time_start, 0);
}

TEST_F(BlueskyCommMockTest, GetScanHistory) {
    BlueskyComm comm(context, "127.0.0.1:60616");
    QString msg;
    std::vector<BlueskyPlan> finished_plans;

    bool result = comm.get_scan_history(msg, finished_plans);

    EXPECT_TRUE(result);
    EXPECT_GT(finished_plans.size(), 0);

    BlueskyPlan& plan = finished_plans[0];
    EXPECT_EQ(plan.type, "count");
    EXPECT_EQ(plan.result.exit_status, "completed");
    EXPECT_GT(plan.result.time_start, 0);
    EXPECT_GT(plan.result.time_stop, plan.result.time_start);
}
*/

//---------------------------------------------------------------------------
// Standalone tests that don't require server connection
//---------------------------------------------------------------------------

TEST_F(BlueskyCommMockTest, ServerStartsAndStops) {
    // This test verifies the mock server can start and stop cleanly
    EXPECT_NO_THROW({
        MockBlueskyServer temp_server("tcp://127.0.0.1:60617");
        temp_server.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        temp_server.stop();
    });
}

//---------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
