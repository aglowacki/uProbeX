### Install

Add flathub remote if first time installing
```bash
flatpak remote-add --user --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
```
Install uProbeX flatpak
```bash
flatpak install --user uProbeX_v1.9.9_Linux_x86_64.flatpak
```

Select 'Y' for downloading aditional kde images.


### Run 

```bash
flatpak run org.UChicagoLLC.ANL.APS.uProbeX
```

You can set an alias in .bashrc
```bash
alias uProbeX="flatpak run org.UChicagoLLC.ANL.APS.uProbeX"
```
