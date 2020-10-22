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

#ifndef CONFIG_CURVE_NUMS384E_H
#define CONFIG_CURVE_NUMS384E_H

#include"core.h"
#include"config_field_F384PM.h"

// ECP stuff

#define CURVETYPE_NUMS384E EDWARDS
#define PAIRING_FRIENDLY_NUMS384E NOT
#define CURVE_SECURITY_NUMS384E 192

#if PAIRING_FRIENDLY_NUMS384E != NOT
#define USE_GLV_NUMS384E   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_NUMS384E /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_NUMS384E /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_NUMS384E 
#define SIGN_OF_X_NUMS384E 

#define ATE_BITS_NUMS384E 
#define G2_TABLE_NUMS384E 

#endif


#if CURVE_SECURITY_NUMS384E == 128
#define AESKEY_NUMS384E 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_NUMS384E SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS384E == 192
#define AESKEY_NUMS384E 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_NUMS384E SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_NUMS384E == 256
#define AESKEY_NUMS384E 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_NUMS384E SHA512  /**< Hash type */
#endif


namespace NUMS384E_BIG = B384_56;
namespace NUMS384E_FP = F384PM;

#endif
