# QcomBaseband

Qualcomm baseband bridge for XNU and iPadOS, with the SC7180 IPA v4.2 data path.

The implementation is by **HoshinoYumeka ([@chiba233](https://github.com/chiba233))**. It is extracted from [Trogdor-ipadOS](https://github.com/chiba233/Trogdor-ipadOS) at `a22feb7f759b4ab0dcedcf1837c4c4792bffa518`. Original commits, authors and the BSD-2-Clause license are retained. [UPSTREAM.json](UPSTREAM.json) maps each imported file to its source path and hash.

## Implementation

| Directory | Contents |
| --- | --- |
| `baseband/` | QMUX/QRTR, QMI CTL, SAHARA, AppleBaseband services, IPA data-port setup, activation indications and eSIM Tape/UIM. |
| `ipa/` | IPA v4.2 configuration, GSI, command/RX channels, QMI startup handshake and QMAP framing. |
| `platform/coachz/` | SC7180 DMA layout, startup/poll wiring and the XNU firmware/service integration patch. |
| `tests/` | Imported protocol and register models. |

The Coachz implementation runs on iPadOS 17 (21H461). With the device's factory FSG data, About shows IMEI, ICCID and modem firmware. A valid SIM registers on LTE roaming with CS/PS attached. The status bar displays the operator and signal bars after SIM reinsertion; cold-boot SIM state and carrier settings remain open.

The bridge opens the IPA data port through DPM, configures WDA raw IP/QMAPv4, and translates the Apple PCIe endpoint to the modem's embedded endpoint. DPM/WDA setup and WDS/QOS binding succeed on Coachz. Activation indications include service-provisioned and accepted-slot states. `pdp_ip` packet delivery and a working cellular data session remain unfinished; EFS writes currently live in memory until reboot.

The eSIM Tape bridge discovers actual eUICC slots, adds UIM capability fields, maps VINYL requests to ISD-R logical-channel APDUs, and restores the original slot mapping. The host model currently fails card-status indication and legacy GetCsn cases. Device eSIM provisioning remains unverified. Other Apple-private QMI requests return unsupported. SAHARA consumes the Apple boot exchange while the modem runs its Qualcomm firmware.

## Build

From a UniXNU resolved target tree with XNU compile records:

```sh
python3 drivers/QcomBaseband/scripts/build.py --tree .
```

This builds `build/qcom-baseband/libQcomBaseband.a` with the baseband IOKit bridge. Add `--coachz` to include the SC7180 IPA configuration, GSI and board adapter. `coachz_modem.c` includes the command, RX and modem implementations in one translation unit. The build uses the selected kernel's compile records.

A board provides modem power/firmware startup, SMEM/SMP2P, GLINK/QRTR transport and rmtfs. It calls the bridge through [QcomBaseband.h](baseband/QcomBaseband.h). See [Coachz integration](platform/coachz/README.md) and the [porting guide](https://unixnu.com/porting/baseband).

Run the imported host models with `python3 scripts/check.py`.

Vendor modem firmware, EFS/NV data and Apple `.bbfw` files are external build inputs. Record their source and destination in the device repository's `proprietary-files.txt`.
