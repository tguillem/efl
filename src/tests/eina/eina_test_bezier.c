/* EINA - EFL data type library
 * Copyright (C) 2015 Subhransu Mohanty <sub.mohanty@samsung.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>
#include <stdio.h>

#include "eina_suite.h"
#include "Eina.h"

START_TEST(eina_bezier_test_values)
{
   Eina_Bezier b;
   double sx, sy, cx1, cy1, cx2, cy2, ex, ey;

   eina_init();
   eina_bezier_values_set(&b,
                          1, 2,
                          3, 4,
                          5, 6,
                          7, 8);
   eina_bezier_values_get(&b,
                          &sx, &sy,
                          &cx1, &cy1,
                          &cx2, &cy2,
                          &ex, &ey);
   fail_if(sx != 1 ||
           sy != 2 ||
           cx1 != 3 ||
           cy1 != 4 ||
           cx2 != 5 ||
           cy2 != 6 ||
           ex != 7 ||
           ey != 8);
   eina_shutdown();
}
END_TEST

START_TEST(eina_bezier_test_angle)
{
   Eina_Bezier b;
   double angle;

   eina_init();
   eina_bezier_values_set(&b,
                          1, 1,
                          3, 1,
                          5, 1,
                          7, 1);
   angle = eina_bezier_angle_at(&b, 0.5);

   fail_if(angle != 0);

   eina_bezier_values_set(&b,
                          1, 2,
                          1, 4,
                          1, 6,
                          1, 8);
   angle = eina_bezier_angle_at(&b, 0.5);
   fail_if(floor(angle) != 90);

   eina_shutdown();
}
END_TEST

START_TEST(eina_bezier_test_length)
{
   Eina_Bezier b;
   double length;

   eina_init();
   eina_bezier_values_set(&b,
                          1, 1,
                          3, 1,
                          5, 1,
                          7, 1);
   length = eina_bezier_length_get(&b);
   fail_if(floor(length) != 6);

   eina_bezier_values_set(&b,
                          1, 1,
                          1, 1,
                          1, 1,
                          1, 1);
   length = eina_bezier_length_get(&b);
   fail_if(length != 0);

   eina_shutdown();
}
END_TEST

START_TEST(eina_bezier_test_t_at)
{
   Eina_Bezier b;
   double length, t;

   eina_init();
   eina_bezier_values_set(&b,
                          1, 1,
                          3, 1,
                          5, 1,
                          7, 1);
   length = eina_bezier_length_get(&b);
   t = eina_bezier_t_at(&b, 0);
   fail_if(floor(t) != 0);

   t = eina_bezier_t_at(&b, length);
   fail_if(t != 1);

   eina_shutdown();
}
END_TEST

START_TEST(eina_bezier_test_point_at)
{
   Eina_Bezier b;
   double x, y;

   eina_init();
   eina_bezier_values_set(&b,
                          1, 2,
                          3, 4,
                          5, 6,
                          7, 8);
   eina_bezier_point_at(&b, 0, &x , &y);
   fail_if(x != 1 ||
           y != 2);

   eina_bezier_point_at(&b, 1, &x , &y);

   fail_if(x != 7 ||
           y != 8);

   eina_shutdown();
}
END_TEST

START_TEST(eina_bezier_test_split_at_length)
{
   Eina_Bezier b, l , r;
   double len, len1, len2;

   eina_init();
   eina_bezier_values_set(&b,
                          1, 2,
                          3, 4,
                          5, 6,
                          7, 8);
   len = eina_bezier_length_get(&b);
   eina_bezier_split_at_length(&b, len/3, &l, &r);
   len1 = eina_bezier_length_get(&l);
   len2 = eina_bezier_length_get(&r);

   fail_if(len != (len1 + len2));

   eina_shutdown();
}
END_TEST

void
eina_test_bezier(TCase *tc)
{
   tcase_add_test(tc, eina_bezier_test_values);
   tcase_add_test(tc, eina_bezier_test_angle);
   tcase_add_test(tc, eina_bezier_test_length);
   tcase_add_test(tc, eina_bezier_test_t_at);
   tcase_add_test(tc, eina_bezier_test_point_at);
   tcase_add_test(tc, eina_bezier_test_split_at_length);
}
