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
flatpak run gov.anl.aps.uProbeX
```

You can set an alias in .bashrc
```bash
alias xrf_maps="flatpak --share=network --filesystem=host --command=/app/bin/xrf_maps run gov.anl.aps.uProbeX"
alias uProbeX="flatpak --share=network --filesystem=host run gov.anl.aps.uProbeX"

```
