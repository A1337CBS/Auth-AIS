/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

#ifndef CONFIG_CURVE_NIST521_H
#define CONFIG_CURVE_NIST521_H

#include"core.h"
#include"config_field_NIST521.h"

// ECP stuff

#define CURVETYPE_NIST521 WEIERSTRASS
#define PAIRING_FRIENDLY_NIST521 NOT
#define CURVE_SECURITY_NIST521 256

#if PAIRING_FRIENDLY_NIST521 != NOT
#define USE_GLV_NIST521   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_NIST521 /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_NIST521 /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_NIST521 
#define SIGN_OF_X_NIST521 

#define ATE_BITS_NIST521 
#define G2_TABLE_NIST521 

#endif


#if CURVE_SECURITY_NIST521 == 128
#define AESKEY_NIST521 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_NIST521 SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_NIST521 == 192
#define AESKEY_NIST521 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_NIST521 SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_NIST521 == 256
#define AESKEY_NIST521 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_NIST521 SHA512  /**< Hash type */
#endif


namespace NIST521_BIG = B528_60;
namespace NIST521_FP = NIST521;

#endif
