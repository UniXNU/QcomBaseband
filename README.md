# QcomBaseband

Qualcomm baseband bridge for XNU and iPadOS, with the SC7180 IPA v4.2 data path.

The implementation is by **HoshinoYumeka ([@chiba233](https://github.com/chiba233))**. It is extracted from [Trogdor-ipadOS](https://github.com/chiba233/Trogdor-ipadOS) at `c5f82b5bc5c73ed55b7f756e6392a99af45c78fb`. Original commits, authors and the BSD-2-Clause license are retained. [UPSTREAM.json](UPSTREAM.json) maps each imported file to its source path and hash.

## Implementation

| Directory | Contents |
| --- | --- |
| `baseband/` | QMUX/QRTR bridge, QMI CTL, SAHARA device endpoint, AppleBaseband methods and IOKit services. |
| `ipa/` | IPA v4.2 configuration, GSI, command/RX channels, QMI startup handshake and QMAP framing. |
| `platform/coachz/` | SC7180 DMA layout, startup/poll wiring and the XNU firmware/service integration patch. |
| `tests/` | Imported protocol and register models. |

The Coachz implementation runs on iPadOS 17.7.10 (21H461). ABM starts, CommCenter exchanges QMI with the modem, the SIM is recognized, and modem firmware appears in About. IPA setup and the QMI initialization handshake complete; modem TX/RX channels start.

The modem's EFS image is empty and it reports no IMEI, including under Linux. CommCenter leaves the SIM disabled. ICCID, network registration and cellular data remain unfinished. The PDP services expose the methods CommCenter expects; `pdp_ip` network-interface delivery and eSIM VINYL are unfinished. Standard QMI services are forwarded; Apple-private QMI services return unsupported. The SAHARA endpoint consumes the Apple boot exchange without loading Apple firmware into the Qualcomm modem.

## Build

From a UniXNU resolved target tree with XNU compile records:

```sh
python3 drivers/QcomBaseband/scripts/build.py --tree .
```

This builds `build/qcom-baseband/libQcomBaseband.a` with the baseband IOKit bridge. Add `--coachz` to include the SC7180 IPA configuration, GSI and board adapter. `coachz_modem.c` includes the command, RX and modem implementations in one translation unit. The build uses the selected kernel's compile records.

A board provides modem power/firmware startup, SMEM/SMP2P, GLINK/QRTR transport and rmtfs. It calls the bridge through [QcomBaseband.h](baseband/QcomBaseband.h). See [Coachz integration](platform/coachz/README.md) and the [porting guide](https://unixnu.com/porting/baseband).

Run the imported host models with `python3 scripts/check.py`.

Vendor modem firmware, EFS/NV data and Apple `.bbfw` files are external build inputs. Record their source and destination in the device repository's `proprietary-files.txt`.
