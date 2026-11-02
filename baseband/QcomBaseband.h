/* SPDX-License-Identifier: BSD-2-Clause */
#ifndef QCOM_BASEBAND_H
#define QCOM_BASEBAND_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
int coachz_baseband_enabled(void);
void coachz_baseband_configure(void (*send)(uint32_t, uint32_t, uint32_t, const void *, uint32_t), void (*del_client)(uint32_t));
void coachz_baseband_new_server(uint32_t service, uint32_t version_instance, uint32_t node, uint32_t port);
void coachz_baseband_del_server(uint32_t service, uint32_t node, uint32_t port);
int coachz_baseband_qrtr(uint32_t dst_port, uint32_t src_node, uint32_t src_port, const void *body, uint32_t size);
void coachz_baseband_poll(void);
void coachz_baseband_report(unsigned second);
#ifdef __cplusplus
}
#endif
#endif
