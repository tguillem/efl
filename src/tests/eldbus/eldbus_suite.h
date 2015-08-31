#ifndef _ELDBUS_SUITE_H
#define _ELDBUS_SUITE_H

#include <check.h>

void eldbus_test_eldbus_init(TCase *tc);
void eldbus_test_eldbus_model(TCase *tc);
void eldbus_test_eldbus_model_connection(TCase *tc);
void eldbus_test_eldbus_model_object(TCase *tc);
void eldbus_test_eldbus_model_proxy(TCase *tc);
void eldbus_test_fake_server_eldbus_model_proxy(TCase *tc);
void eldbus_test_eldbus_model_method(TCase *tc);
void eldbus_test_eldbus_model_signal(TCase *tc);

#endif
