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

#ifndef CONFIG_CURVE_BRAINPOOL_H
#define CONFIG_CURVE_BRAINPOOL_H

#include"core.h"
#include"config_field_BRAINPOOL.h"

// ECP stuff

#define CURVETYPE_BRAINPOOL WEIERSTRASS
#define PAIRING_FRIENDLY_BRAINPOOL NOT
#define CURVE_SECURITY_BRAINPOOL 128

#if PAIRING_FRIENDLY_BRAINPOOL != NOT
#define USE_GLV_BRAINPOOL   /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_BRAINPOOL /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_BRAINPOOL /**< Not patented, so probably safe to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_BRAINPOOL 
#define SIGN_OF_X_BRAINPOOL 

#define ATE_BITS_BRAINPOOL 
#define G2_TABLE_BRAINPOOL 

#endif


#if CURVE_SECURITY_BRAINPOOL == 128
#define AESKEY_BRAINPOOL 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_BRAINPOOL SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_BRAINPOOL == 192
#define AESKEY_BRAINPOOL 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_BRAINPOOL SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_BRAINPOOL == 256
#define AESKEY_BRAINPOOL 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_BRAINPOOL SHA512  /**< Hash type */
#endif


namespace BRAINPOOL_BIG = B256_56;
namespace BRAINPOOL_FP = BRAINPOOL;

#endif
