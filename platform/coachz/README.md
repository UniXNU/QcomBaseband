# Coachz integration

The SC7180 example is by **HoshinoYumeka ([@chiba233](https://github.com/chiba233))**. Its IPA register tables target v4.2. `coachz_modem.c` contains the board's fixed DMA ranges and assumes the apps SMMU is in bypass.

## Startup order

1. Provide modem power, clocks, interconnect votes, reserved MBA/MPSS memory, SMEM/SMP2P, GLINK/QRTR and rmtfs from the SoC/board integration.
2. Call `coachz_modem_configure(qmp_send, send)` after IPA power is available and before starting the modem. Call `coachz_modem_starting()` as modem startup begins.
3. On SMP2P `ipa-setup-ready`, call `coachz_modem_setup()`. Forward QRTR service announcements to `coachz_modem_new_server()` and IPA QMI traffic to `coachz_modem_qmi()`. Poll `coachz_modem_poll()` from the transport worker.
4. Set `/chosen/coachz-baseband` and publish the selected OS's `/baseband` properties and `/product/baseband-chipset`. The reference uses root `compatible = J121AP` for the Mav16 Qualcomm branch while keeping display identity properties unchanged.
5. Call `coachz_baseband_configure(send, del_client)`. The callback sends a QMI body from a local QRTR port to a remote node/port. Forward QRTR NEW_SERVER, DEL_SERVER and replies through the functions in `baseband/QcomBaseband.h`; poll `coachz_baseband_poll()` on the same worker. The reference worker polls every millisecond.
6. Apply the board's XNU service/firmware integration. `patches/coachz-prelink-baseband.patch` targets XNU 10063 after the Coachz Wi-Fi and OS patches: it connects the QRTR hooks, enables the cellular CommCenter job and mounts the Mav16 firmware directories before launchd.
7. Compile with `--coachz`, link the archive into the source kernel, and register its IOKit classes through the selected kernel's class-initialization path before publishing services.

The board packages its Qualcomm modem firmware separately from the Apple Mav16 files used by ABM and BBUpdater. The reference Apple archive has magic `CZBBFW01`; the XNU integration checks its hash, writes changed files under `/private/var/coachz/baseband/Mav16`, and exposes them at ABM's standalone and personalized firmware paths.

## Checkpoints

Check modem startup, QRTR services, IPA INIT_DRIVER/INIT_COMPLETE, AppleBaseband publication, ABM boot, QMUX clients, SIM identity, registration and a data session in that order. The device's factory FSG supplies modem identity and calibration; the reference reads it from the eMMC hardware boot partition and verifies it against the device's stored digest. Firmware packages keep this device-specific input with the board.

`baseband/coachz_bb_data.h` opens the DPM hardware port, negotiates WDA raw IP/QMAPv4 and translates WDS/QOS endpoints from PCIe `{3, 4}` to embedded `{4, 1}`. The activation indication carries service-provisioned and accepted-slot TLVs. Coachz accepts the data-port bindings and registers a valid SIM on LTE roaming with CS/PS attached. The operator and signal bars appear after SIM reinsertion. Cold-boot SIM state, carrier settings, EFS write persistence and `pdp_ip` delivery remain open.

`baseband/coachz_bb_esim.h` connects VINYL Tape to UIM: actual eUICC discovery, capability TLVs, segmented requests/results, ISD-R logical channels and APDUs, timeouts, and original-slot restoration. The host model currently fails card-status indication and legacy GetCsn cases; the real eUICC/RSP session remains unverified.

On another board, supply its DMA ownership, address translations, modem firmware, register generation and transport implementation. Keep the reference class names and channel protocol properties aligned with the selected OS ABI.
