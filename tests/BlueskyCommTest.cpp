/*-----------------------------------------------------------------------------
 * Copyright (c) 2024, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mvc/BlueskyComm.h"
#include "mvc/BlueskyPlan.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <zmq.hpp>

using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgReferee;

//---------------------------------------------------------------------------
// Mock ZMQ Socket for testing
//---------------------------------------------------------------------------

class MockZmqContext : public zmq::context_t {
public:
    MockZmqContext() : zmq::context_t(1) {}
};

//---------------------------------------------------------------------------
// Test Fixture
//---------------------------------------------------------------------------

class BlueskyCommTest : public ::testing::Test {
protected:
    zmq::context_t* context;
    QString test_ip;

    void SetUp() override {
        context = new zmq::context_t(1);
        test_ip = "127.0.0.1";
    }

    void TearDown() override {
        delete context;
    }

    // Helper function to create a mock success response
    QByteArray createSuccessResponse(const QString& msg = "Success") {
        QJsonObject response;
        response["success"] = true;
        response["msg"] = msg;
        return QJsonDocument(response).toJson();
    }

    // Helper function to create a mock failure response
    QByteArray createFailureResponse(const QString& msg = "Failed") {
        QJsonObject response;
        response["success"] = false;
        response["msg"] = msg;
        return QJsonDocument(response).toJson();
    }
};

//---------------------------------------------------------------------------
// Constructor Tests
//---------------------------------------------------------------------------

TEST_F(BlueskyCommTest, ConstructorWithValidIP) {
    // This test verifies the constructor creates a connection with valid IP
    EXPECT_NO_THROW({
        BlueskyComm comm(context, test_ip);
    });
}

TEST_F(BlueskyCommTest, ConstructorWithEmptyIP) {
    // This test verifies the constructor handles empty IP gracefully
    QString empty_ip = "";
    EXPECT_NO_THROW({
        BlueskyComm comm(context, empty_ip);
    });
}

//---------------------------------------------------------------------------
// Message Generation Tests
//---------------------------------------------------------------------------

TEST_F(BlueskyCommTest, GenSendMesgWithoutParams) {
    BlueskyComm comm(context, "");
    QByteArray result = comm.gen_send_mesg("test_method", nullptr);

    QJsonDocument doc = QJsonDocument::fromJson(result);
    QJsonObject obj = doc.object();

    EXPECT_EQ(obj["method"].toString(), "test_method");
    EXPECT_FALSE(obj.contains("params"));
}

TEST_F(BlueskyCommTest, GenSendMesgWithParams) {
    BlueskyComm comm(context, "");
    std::map<QString, QString> params;
    params["key1"] = "value1";
    params["key2"] = "value2";

    QByteArray result = comm.gen_send_mesg("test_method", &params);

    QJsonDocument doc = QJsonDocument::fromJson(result);
    QJsonObject obj = doc.object();

    EXPECT_EQ(obj["method"].toString(), "test_method");
    EXPECT_TRUE(obj.contains("params"));

    QJsonObject jparams = obj["params"].toObject();
    EXPECT_EQ(jparams["key1"].toString(), "value1");
    EXPECT_EQ(jparams["key2"].toString(), "value2");
}

TEST_F(BlueskyCommTest, GenSendMesg2WithParams) {
    BlueskyComm comm(context, "");
    QJsonObject params;
    params["test_key"] = "test_value";
    params["number"] = 42;

    QByteArray result = comm.gen_send_mesg2("test_method", params);

    QJsonDocument doc = QJsonDocument::fromJson(result);
    QJsonObject obj = doc.object();

    EXPECT_EQ(obj["method"].toString(), "test_method");
    EXPECT_TRUE(obj.contains("params"));
    EXPECT_EQ(obj["params"].toObject()["test_key"].toString(), "test_value");
    EXPECT_EQ(obj["params"].toObject()["number"].toInt(), 42);
}

//---------------------------------------------------------------------------
// Plan to JSON Item Tests
//---------------------------------------------------------------------------

TEST_F(BlueskyCommTest, PlanToJsonItemBasic) {
    BlueskyComm comm(context, "");
    BlueskyPlan plan;
    plan.name = "Test Plan";
    plan.type = "count";
    plan.uuid = "test-uuid-123";

    QJsonObject result = comm.plan_to_json_item(plan);

    EXPECT_EQ(result["name"].toString(), "count");
    EXPECT_EQ(result["item_type"].toString(), "plan");
    EXPECT_EQ(result["item_uid"].toString(), "test-uuid-123");
    EXPECT_TRUE(result.contains("meta"));
    EXPECT_EQ(result["meta"].toObject()["name"].toString(), "Test Plan");
}

TEST_F(BlueskyCommTest, PlanToJsonItemWithParameters) {
    BlueskyComm comm(context, "");
    BlueskyPlan plan;
    plan.name = "Test Plan";
    plan.type = "scan";

    BlueskyParam param1;
    param1.name = "detector";
    param1.default_val = "det1";
    param1.kind = BlueskyParamType::String;

    BlueskyParam param2;
    param2.name = "num_points";
    param2.default_val = "100";
    param2.kind = BlueskyParamType::Double;

    BlueskyParam param3;
    param3.name = "save_data";
    param3.default_val = "true";
    param3.kind = BlueskyParamType::Bool;

    plan.parameters.push_back(param1);
    plan.parameters.push_back(param2);
    plan.parameters.push_back(param3);

    QJsonObject result = comm.plan_to_json_item(plan);

    EXPECT_TRUE(result.contains("kwargs"));
    QJsonObject kwargs = result["kwargs"].toObject();
    EXPECT_EQ(kwargs["detector"].toString(), "det1");
    EXPECT_EQ(kwargs["num_points"].toDouble(), 100.0);
    EXPECT_EQ(kwargs["save_data"].toBool(), true);
}

TEST_F(BlueskyCommTest, PlanToJsonItemWithoutUUID) {
    BlueskyComm comm(context, "");
    BlueskyPlan plan;
    plan.name = "Test Plan";
    plan.type = "count";
    plan.uuid = ""; // Empty UUID

    QJsonObject result = comm.plan_to_json_item(plan);

    EXPECT_FALSE(result.contains("item_uid"));
}

//---------------------------------------------------------------------------
// Parse Args Tests
//---------------------------------------------------------------------------

TEST_F(BlueskyCommTest, ParseArgsWithMixedTypes) {
    BlueskyComm comm(context, "");
    BlueskyPlan plan;

    QJsonObject kwargs;
    kwargs["string_param"] = "test_string";
    kwargs["double_param"] = 3.14;
    kwargs["int_param"] = 42;
    kwargs["bool_param"] = true;

    comm.parse_args(kwargs, plan);

    EXPECT_EQ(plan.parameters.size(), 4);

    // Find and verify each parameter
    for (const auto& param : plan.parameters) {
        if (param.name == "string_param") {
            EXPECT_EQ(param.default_val, "test_string");
        } else if (param.name == "double_param") {
            EXPECT_EQ(param.default_val.toDouble(), 3.14);
        } else if (param.name == "int_param") {
            EXPECT_EQ(param.default_val.toInt(), 42);
        } else if (param.name == "bool_param") {
            EXPECT_EQ(param.default_val.toLower(), "true");
        }
    }
}

TEST_F(BlueskyCommTest, ParseArgsWithEmptyObject) {
    BlueskyComm comm(context, "");
    BlueskyPlan plan;

    QJsonObject kwargs; // Empty object

    comm.parse_args(kwargs, plan);

    EXPECT_EQ(plan.parameters.size(), 0);
}

//---------------------------------------------------------------------------
// Integration Tests (These would require actual ZMQ mocking)
//---------------------------------------------------------------------------

// Note: The following tests are conceptual and would require proper ZMQ mocking
// or a test server to actually run. They demonstrate what should be tested.

/*
TEST_F(BlueskyCommTest, OpenEnvSuccess) {
    // Would require mocking ZMQ socket behavior
    // BlueskyComm comm(context, test_ip);
    // QString msg;
    // bool result = comm.open_env(msg);
    // EXPECT_TRUE(result);
    // EXPECT_EQ(msg, "Environment opened");
}

TEST_F(BlueskyCommTest, CloseEnvSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, StartQueueSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, StopQueueSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, QueuePlanSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, UpdatePlanSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, MovePlanSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, RemovePlanByPositionSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, RemovePlanByUUIDSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, GetAvailScansSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, GetQueuedScansSuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, ClearHistorySuccess) {
    // Would require mocking ZMQ socket behavior
}

TEST_F(BlueskyCommTest, GetScanHistorySuccess) {
    // Would require mocking ZMQ socket behavior
}
*/

//---------------------------------------------------------------------------
// Error Handling Tests
//---------------------------------------------------------------------------

TEST_F(BlueskyCommTest, OperationsWithNullSocket) {
    // Create comm with empty IP to get null socket
    BlueskyComm comm(context, "");
    QString msg;

    // All operations should return false with null socket
    EXPECT_FALSE(comm.open_env(msg));
    EXPECT_FALSE(comm.close_env(msg));
    EXPECT_FALSE(comm.start_queue(msg));
    EXPECT_FALSE(comm.stop_queue(msg));
    EXPECT_FALSE(comm.clear_history(msg));

    BlueskyPlan plan;
    EXPECT_FALSE(comm.queue_plan(msg, plan));
    EXPECT_FALSE(comm.update_plan(msg, plan));
    EXPECT_FALSE(comm.movePlan(msg, 0, 1));

    BlueskyPlan out_plan;
    EXPECT_FALSE(comm.removePlan(msg, 0, out_plan));
    EXPECT_FALSE(comm.removePlan(msg, "test-uuid"));

    std::map<QString, BlueskyPlan> plans;
    EXPECT_FALSE(comm.get_avail_scans(plans, msg));

    std::vector<BlueskyPlan> queued_plans;
    BlueskyPlan running_plan;
    EXPECT_FALSE(comm.get_queued_scans(msg, queued_plans, running_plan));

    std::vector<BlueskyPlan> finished_plans;
    EXPECT_FALSE(comm.get_scan_history(msg, finished_plans));
}

//---------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
