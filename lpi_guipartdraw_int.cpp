/*
Copyright (c) 2005-2009 Lode Vandevenne
All rights reserved.

This file is part of Lode's Programming Interface.

Lode's Programming Interface is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lode's Programming Interface is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lode's Programming Interface.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "lpi_guipartdraw_int.h"

#include "lodepng.h"
#include "lpi_draw2dgl.h"
#include "lpi_draw2d.h"
#include "lpi_file.h"
#include "lpi_base64.h"
#include "lpi_xml.h"

#include "lpi_input_sdl.h"

#include <iostream>

namespace lpi
{
namespace gui
{

////////////////////////////////////////////////////////////////////////////////
//TEXTURESET////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
PNG image, encoded in base64, containing all the GUI images
First use the function decodeBase64
Then use the function LodePNG::Decoder::decode
And you get the pixels in a buffer
*/
const std::string builtInGuiData = "\
iVBORw0KGgoAAAANSUhEUgAAAQAAAAIACAIAAADi+CWsAAAY2UlEQVR42u2dvc8bxdqHdx8FAVUa\n\
GhqKNGnS8BfYkWg4EopeCQregiLFSRPpFUJ6kQ5ItiVyJJBQhJQmFCnSpCBNhAQNEn5amjRp0lCk\n\
oaFJfRA+Y48zz+zMfszuzHo/7utSeFiP9/bY6/u3M7M7P0++zNbZiPl1t7qeb4Z+FzBbzrJDkpnH\n\
alv/c8rNU6a8cR/nZZ0dSl+nKhygJ878otIzrj4T15+MnX3Uhslms22Xl2KHD31wYP6UCKAmO0PO\n\
yvYJ3iSx0xqUltvUiwQgFUcB2F2OqjN9aZemZh/nrG+Xl27br0D2w2nIGQSDZC5xrgXJjL0FAOiV\n\
s/iXAJguCABEgwBANAgARIMAQDQIAESDAEA0CABEc1YzJ7l0u2p/Z5JzY+wUcT7FySZv+4f99O9h\n\
rpS3AFWzl6um5dizO2tmPk99Vo9+/6UTtvuevlozg9Cffw7huIaYcKomh9bMcB76w/ZIr5P2nMy2\n\
67IrZeJgB5rHAIHT9xuNLFU9q+lyssbNP1alR5IWoANRk+EKhzvP9393u5qSwukqa/dVXc+Gj91a\n\
x0p/qtza0y+xj21M7BSP1VRiL7Xa2+f8/FxvLIoP/ZLFYhFZ1xjYbrfHreWy8NArWR4e2qzXa7NV\n\
eOiVFJ6CPokVAITz2muv1TwsLYG+QQCn4xQC2BU7UtBEnADy3OnWLLx2v1iyHvrzDknvAthlm81m\n\
1bITLBxagHa8/vrrNQ9LSwz9CkBn/5rsb0ecAHa7izHu4Ux/bg0KnZL9IHj6V6nfeOONmoelJYYe\n\
BUD2d4UWoB1jFICd/YwBWoIA2vHmm2/WPCwtMVy6dKnmYWlJM8XsZwzQFgTQjnG1AH720wtqCVeB\n\
2jEiAZD9KaAFaMdYBFCV/YwBWsJVoHaM4jJodfYzBmgLLcDpSCOA+uynF9QSBHA6EgiA7E9NrACc\n\
OZ7+lM95TAI15Pmxi70rPiwtcTDzQ3fFh6UlFdVnFz2cqm1oQ77b7bSRwv6bvZq7729nr4wXzl/9\n\
lP86Wa3BsqougNOQxhNc6vCyHbSBblqyH05MGk9wksTF0QenJ40nOMQUX7V/q0CAtMR5gifi+ySW\n\
2Cr4ZTgQTZr7AHnt/fddtiN20rEzJoEA1JH96aef/mPx119/2Q/z/8+rji+x44+dN8nuBN+4cePx\n\
48dO4aeffvrNN980xl6+fPnPP/90Ct99993ffvtt6OPTwMcff+yUhHxezdtvv+2UhHxedZxLywOP\n\
c2n5+I9zfyQbAzx58uTDDz+0S27fvn337t2Q2JcvX7711lt2ybVr154+fTr0wYH5k3IukGoBlAYe\n\
Pnyotm/dunXv3j3VtgbGqhZAnRFfvHihtq9evfrs2bOhj0wo6vOa7oTSfKtY9XlNrNJ8YJQ6zk43\n\
JrxedZyd2PB6Z0niyXCPHj3SvYL79++HZ7/mjz/+eOedd9TG8+fPhz4szZiuyCeffKI3lOADY01X\n\
RH9exYQEPzO4DJqMtqd/G+Gn4QFJ3AKo07/uEty8eTP8jKjRXSAVO5UukN0V0Z9XPwyJtbsiV65c\n\
UZ+3bYMJSUgpADUAUF0g/UWqLpAaBgQOghVqEKy6QHpbdYFaDYKdwXcr4TmD70kIDxKS+DKofRpT\n\
iah6Bd0ug6pEHP9lUEd4rXCEN4l6Z0kyATx58sRvxFULENKyv3z50i8MbwH8qyLhb9u5KtL283ar\n\
V31eJzay3sDa/XrbHq75kUAAu2yX/6PjbXZixx87b9K0ADGHj9jxx84YLoOCaI6eYP0gxtELMEWO\n\
lsgkjl6AybEXAKdwEItriu/s6AWYIqdbJ3iz2WytqpbW9uqwKujGWhvUL7Fjp+I3JXb8sadbJ9jH\n\
+SE0/3fRTMlisdiu6adBergMCqJBACCa060Qo/bcWuONldW/b4w94O4PEA8tAIjmdCvEbLd2A7C/\n\
wnMxxpWxugyMEFoAEA0CANEgABANV4FANLQAIBquAoFoaAFANAgARBPVBdpsLrolejS8tbsxXomD\n\
tDWGYYS46wRnRbNv/TrB+mHNtr1/hvUMxofrCPONv23X8bUdxriHYeQUPMGlOWqf1EOwd3bMlrQA\n\
MDYKg+BSc7CtCmcHu7x0H1oAGDlRnuD1dtlu/+XWbP+arerXLbTZZTvH9yktNjBQQ2w4idcHCEdl\n\
w3a7/Y+H84OvxzUM1wWhSouF/uA+AIgGAYBoEACIZlYCuHnzZufYK1eudIjyF8oOx18oG07PfARg\n\
livtgFmudEL1QhJmIoBhz8Snb3kgFXMQwBgWjbt161bn2KtXryY/JhDIHATw+PHjzrHO6pSduX//\n\
fufY58+fJz8mEMgcBKB49OhR51izPnFnHjx40Dn2999/7+mYQAgzEYDi4cOHnWNfvHgxuXohCfMR\n\
QDbEmXjYlgfimZUAANqCAEA0g80G3a9dvuw4PVhaLPQHnmAQjdsFwhMMoigIAE8wSOPM7s/gCQZp\n\
4AmeRmxgoIbYcPAETyAW+oP7ACAaBACiQQAgGgQAopmPAPDmQgcGuwqUFu0JvnnzZgdnlvYEX7ly\n\
pZUz68aNG37h3bt3Q2IvX77sFz59+vRURwsumEMLYHuC23pzbU9wK2/ukydPnJJ79+4Fxr58+dIp\n\
efbsWf/HCUqYvAD8M/Ht27cDY/0z8bVr18Krtr3IbVse24uMJ3hAJt8FUmfi0ptKIbHqTOzHtqr9\n\
0aNHqvf14MGDtoHZwRGmel94godl8i3A4OAJnjQIAESDAEA0CABEgyd4ArHQH3tPcNa/D7jUXuN7\n\
kev3dPap2h/fGYTjLpNqNpL7gJ1y5xWcPf23lFVIouadAzRydjIfcKnfslWNVfs4tdMCQDhnJ/MB\n\
V53XS2useuXSs7vT/tACQDhDeoJbxU7Rb0rs+GO5DAqiQQAgGgQAokEAIBoEAKJBACAaBACiQQAg\n\
GgQAokEAIBoEAKJBACAaBACiQQAgGgQAoil4gh2bL9ZbmD0FT7AP1luYN2fhqYz1FubHWZXl15TY\n\
/SKstzAzojzBRf+l/tWnXU1JvXfTXkZ3V3idktjNerNar6xa6rhufcbIelsdH2LHHxv7y3AqEfXG\n\
al14WFpSpTWdgt9///2lV+T/uy/x07HALiT56+hYL8yIOAHslNxeCW69Vn9WK0t/Tkmebct+7k9l\n\
4XfffaeSzy784Ycf9un4P3l/uThUvTAq4u4D5Na/xpLyF8i//fbbqpf/8ccf88iTfOUbH6ZeGBuR\n\
N8JaKSD3gvOvv/66voKff/65LBfzlWpe8rxbHyiiXpgb3AkG0QwmAHV+/eqrr0L2/OWXX7yT8W6j\n\
WoD9PezWPfW4emFu0AKAaBAAiAYBgGgGE8Au23355Zche7733nveVfnuV4Hi6oW5QQsAohlSAOr8\n\
+vnnn9fv8/7775edhrtfBYqrF+bGwC2ASrLPPvus6tkPPvigpywcql4YG4Mtk2rYrx/6f8dJaabw\n\
o48+ynqelDZUvTAqhhdA9irh8n/WTUt22M+FjvYjdKgXZoa7TnBmmYCdjazMCGavLoylGCZHuSfY\n\
zuBGsBTDdDmrWss6PPuxFMN0OatKXydfSxfxzapXETYlWIphzAzmCV4Wq31l8D1iWX5LYmM8wU69\n\
Ne+htN5Wx4fY8ccO6Qk2bsnNZuO8pnpqk21KM/KCrp7ggm/TrrexRpgdA3uCz8/PnRKd/ap8uVz6\n\
7UAq/HoVvdYI4yROAHmnkrGy3W7JfmlEdoHaKmCd6G3n+/7VuuOrOT0uA9kvkFHcCT4lZDnYTHQ6\n\
dNRsUADDRAUAkAZxXSBN4eJsEfpIohhMACrPNtlmuVye/tqLvtJa/tSGCRqyGLIF2Gug7Lp7QBZ2\n\
vwpk7jMM+MFhPAzcBfKzv+/WQLc8aAA0Ex0D7I7dmP09htZ32rQGhv4IMAomKoBYGOmChsugIJqp\n\
CoBTOCTh6AnOLF+v44f0t52/+qmqfYb+gAB17McANU5I28Vr7+PkvX5oG94xfMEkcLtAdqJXnb+r\n\
bMQGfvcBpsJeAL6XV1NjBbb3Kd2mEYBJEOsJDl9DZZftHO8mseGxgYEaYsOJug+gvtHtdhu687Ig\n\
NmLDY6E/EtwIWywWjfv0Me9AWr3QB1O9D3B89wfCy22WB8LLbTYHwstttgfCy6FXpi2A42co5npj\n\
6ts4ud6Y+jbur7m0mUrt5DqpPxTTFsDff/99/Bivkt5smKeqMDlnkt5sNKaj/4tGZqPKaWDwa/Hf\n\
CZyMaQsgK2ogPPs1duaFZ7/G1kB49pvqTF1k/7BMXgCZl+6B2a+J6Yo46R6Y/ZqYrhckZA4CAOjM\n\
HATAIBg6M3kB2P1+f0xcT0xf3O73V/3KbxUxYw9Iy7QF4I96wzUQczXGH/WGayDm6hMkZ9oC0DAI\n\
hs5M2xNcleshGqjK9RANVOV6iAaqch0NDEICAQw170VavdAHUQLYrzW97DjFl9jwWOiPNOsEZ1iK\n\
YZqkWSfYz2PzVOk+VUtHmg3cZHAaYtcJdsBSDNMidp3gDEsxTJkePcE7b/mWcI8ssVP0104xNtYT\n\
bH5Xq+TZPN9VL2FU75HNlw2xouqF/pjwneD8QNXDehzfY4gN0uD4HkNskAbH94gNcnAmLACNTvrw\n\
1LfRSd/tFqxO+m4ryuikJ/XHwIQFYHojJvtr+icO/oy08HT0572FzwLyq2MGxLBMWABZMePDs1/j\n\
dEVaxdoZ32oOXFbMeLJ/cKYtAGcM0Co2JhGdMUCr2BjhQXImLAC/59NqEKw3OnRF/J5Pq0Fw1RuA\n\
QZiwADQ6+9v2fzQxg1Gd/W37P5qYwTekZcJ+ACfpW2kAHwxooqdD5y3ubrqxy4hYSfVCf8S2ADHf\n\
HLEwOJMfAwDEgABANAgARIMAQDQXnmD92LYCZ0V/cL2NC2CKHD3BOrlr/OymZOg3DJCS4zKpVa52\n\
gHlTaAGy6hXhTcnQbxggJbGeYL/w33f+/a8v/lW6f6N3k9jw2BqIDSfxVSD1jX7xxRfqL7G9xkIq\n\
Uk6G09/odrvdz/G6k1Wd24iNjIWEJBOA+UYXi0Xb79XEZoeJmd1ihdQLaUnTBbKzITuspa62A9t3\n\
Oxs0HWJVjXmed65XT0vuVu9ms+lcr/lLX2goEgjAyX5NYE742a9pFauzPzs4wjrUa9tT2tZrfhui\n\
Q71oYAzECqA0+zWNOVGV/ZrAWJP9mhAN+NmvCdGAn/2aEA342W8+aWO90BNRAqjJfk1NTtRnv6Yx\n\
1sl+Tb0GqrJfU6+BquzX1GugKvvNJ83QwBBECUCN2+7cuaOSpmrRFFWunlX7+CM8E1vz+o2x6vV9\n\
J6QqCay3KhFD6vWdkKoksN4q4VXFQn/EdoFqNFCTDU5s6bOBsY4G6rPfr9fvioTX6/w6UKt6/a4X\n\
2T8ICQbBpRpozAYn1ilvFWs0EJL9fr12t6Rtvea3ITrU6/wl+wchzX2A/Td3Z/8t6v5xYDb4sbqk\n\
W2x49vux4dnvx4Znvx9L9g9Oshthzvfa6hslluwfipRTIcz32uEbJRYGIfEPY+nvtds3SiycnvSe\n\
4JhvlFg4MWnWCcZSDBMlzTrBWIphoiRYJxhLMUyXBOsEYymG6ZLME7xfQrTpx1+rvJvEhseGQGw4\n\
aa4C6UVwaxaC7i9WDeLl1AvJSSAA/Y3qmcBtv1ezfHS32H0W5l1yUder78V2q3ez2XSuV4MGxkCs\n\
AEz2Z69m/4d/r87i6W1jdfZneon2Nrlosj/LsrYaMNmfHQwAHeo1gkcDYyBKAHb2a8I14GS/JjzW\n\
ZP+xJFgDdvZrwjVgZ78mXAN29psPiwYGJ0oAaiRXOgs6ZCkUHesUhsfqjL8o0X2h4HqdRGxVr2MD\n\
aFWvI7zAeqE/YrtAtgbCs9+ONQ/bxhoNhGe/Xa/9qyRt6zU2gA71mq4X2T8GEkyG08u/tc2kNLF5\n\
3jb7p1sv9EGa2aD6e+32jcbGts/C6dYLyUk2G3SKSy9OMRbSwhJJIBoEAKJBACAaBACiQQAgGgQA\n\
ormYUFBvBc4wNMIcOdNpXepet81fZD/MErcLZLsWTdKHG+QBpoUrgFJfPNkPcyXhOsH58b9X7Jz/\n\
13k3Y2PPrenNi+MM09DYzfriCKyO2yGxzRA7/tg0k+EOLu89ayuZssN2owHcxBb8MYckbow9354v\n\
lm7s4uKpRU3sZr1ZrXVVy7KnaPREkMYTXMj7IuqpGtNTqS/MUG+YUileH6t2qHpWpXjN4jTqKbVD\n\
/JGB8ZPAE1yT/ZoqDdRnv6ZKA/XZb2JLNVCf/Ro0IITILtCh91LSh86ckkN/QuXx2o21EnHhJaUu\n\
2ZXFqp7PwqslqywpxK6K+1bVuzgGuq8Mc4I7wSCaqBYgt0a9+ozrX1ExJWrPZTHW9GH0Gde/knNu\n\
LZpkx55bP6/QWO8+dHnxyqrcvFhjvfta1hnMGFoAEA0CANEgABBN1BjAuQSU1V6NWe3v2BVji5df\n\
qq7G6J2vV7xsY70H1jV71tS7UMMABgGzZpItwKLpKj5AIFwF4iqQaCbZAgCkAgGAaGJng66tMW5W\n\
nA3qlyTEmcxT+kPTfdQLM6PgCXZWOPXXfjT72IsBl8bWvA7uShgPx3WCbfdj6X52uSOD0tiQ1wEY\n\
nC5jAH/900DwFsPY2AvAWQbYPFdVXpXHTqz/OmQ/jI2EnuBmCt5NWwx6tS9rySOnZLPZbK23ubS2\n\
G+8DKOzYqXhViT1NbBpPsKa1lTavfWiXrN1nrIrWxYdeyU4JgIEHlJPsPgAGQpgiaQRA9sNESdAF\n\
0tm/Wq1UT91XQmWnKK/p8bglq3VhVuahl78OjD38f50BlBErAJP95m/h2Q0tA4ya2C6QPsHrRN8U\n\
aYjcX+Ex/45FVSWb4nyKw8PQ2MLFJYAiCcYAtgbUtv439OcCCCLNIJiMh4mS7DIoGoApkvJGmNKA\n\
fxXIlLgK4SoQjIDEhhiT5c51IdoHGCfpHWHOdaGsKvu5CgQjoBdLpHNdaOjPCFBJX55gnfdkP4yc\n\
lINgh/rs37cPa7PnocQaQPslLqZfkxcflpYAVLD3BBvnbr3H11lKFXMjzICjI6zqaVZKhXlzNMWb\n\
xyHGdoDZUDcItj3BTvnQbxsgDXGe4Ahf76/ZyrJQ5lkT1633ORW/KbHjj429CnR+flyGcVF8WFpS\n\
wi4k+QH6gt8GBdEgABBNXBcozxfFgjarrRzW6+3np3MBAqEFANHEtQC73cUguPVqK7v9VaDV6jAI\n\
ZiAMw0ALAKJBACAaBACi4SoQiIYWAETDVSAQDS0AiAYBgGiiukD2L+Dq0fDW7gJ5JQ77udA4C2BQ\n\
8ASDaBq6QHiCYd64AsATDKI4q+nP4AmG2TOkJ9h+G4e/db9uNUW/KbHjjx3SE2z9cnrhYWkJv3AO\n\
fdDjTyM2sMsuLoIeZgQV1thzSvJsmzEggfQMJ4AW62MA9MWQs0H9l6stWWcAqWEqBIhmwNmgAMND\n\
CwCiQQAgGgQAohnwKhDA8NACgGi4CgSioQUA0SAAEM2QnmCAwYnyAwBMHbpAIBoEAKJBACCaqEHw\n\
VHyfxBJbBS0AiAYBgGgQAIgGAYBoEACIBgGAaBAAiAYBgGgQAIgGAYBoEACIBgGAaBAAiAYBgGgQ\n\
AIgGTzCIhhYARIMAQDQIAESDJ5hY0bG0ACAaBACiQQAgGgQAokEAIBoEAKJBACAaBACiQQAgGgQA\n\
okEAIBoEAKJBACAaBACiQQAgGjzBIBpaABANAgDRIAAQDZ5gYkXH0gKAaBAAiAYBgGgQAIgGAYBo\n\
EACIBgGAaBAAiAYBgGgQAIgGAYBoEACIBgGAaBAAiAYBgGjwBINoaAFANAgARIMAQDR4gokVHUsL\n\
AKJBACAaBACiQQAgGgQAokEAIBoEAKJBACAaBACiQQAgGgQAokEAIBoEAKJBACAaBACiwRMMoqEF\n\
ANEgABANAgDR4AkmVnQsLQCIBgGAaBAAiAYBgGgQAIgGAYBoEACIBgGAaBAAiAYBgGgQAIgGAYBo\n\
EACIBgGAaBAAiAZPMIiGFgBEgwBANAgARIMnmFjRsbQAIBoEAKJBACAaBACiQQAgGgQAokEAIBoE\n\
AKJBACAaBACiQQAgGgQAokEAIBoEAKJBACAaPMEgGloAEA0CANEgABANnmBiRcfSAoBoEACIBgGA\n\
aBAAiAYBgGgQAIgGAYBoEACIBgGAaBAAiAYBgGgQAIgGAYBoEACIBgGAaPAEg2hoAUA0CABEgwBA\n\
NHiCiRUdSwsAokEAIBoEAKJBACAaBACiQQAgGgQAokEAIBoEAKJBACAaBACiQQAgGgQAokEAIBoE\n\
AKLBEwyioQUA0SAAEA0CANEgABANAgDRHH8V4tddwUt/Pd90eTGAqbEXgMp+J+P9EoBZctEFUkmv\n\
/9klpc/au1VF2TvYe1Zt21F2LVU1VtUO0IrCGECd9WtO/PZTattOu6oo3ZKYPau2a17BlHeIBWgk\n\
wSC4Pv9CTs+N2d+5doB6CgLwuxNOf8MuN5nnROnTs33mLq249BWc2JDuDV0giGE/F4hBMIjlOBmO\n\
y6AgE2aDgmi4Ewyi+S9d0QX+q5uczgAAAABJRU5ErkJggg==\n\
";

void GUIPartDrawerInternal::initBuiltInGui(const ITextureFactory& factory)
{
  initBuiltInGuiTexturesSmall64(factory, builtInGuiData);
}


void GUIPartDrawerInternal::initBuiltInGuiTexturesSmall(const ITextureFactory& factory, const std::string& png_file_name)
{
  std::vector<unsigned char> png;
  loadFile(png, png_file_name);
  if(!png.empty())
    initBuiltInGuiTexturesSmall(factory, png);
}


void GUIPartDrawerInternal::initBuiltInGuiTexturesSmall64(const ITextureFactory& factory, const std::string& png_base64)
{
  std::vector<unsigned char> decoded64;
  decodeBase64(decoded64, png_base64);
  initBuiltInGuiTexturesSmall(factory, decoded64);
}

void GUIPartDrawerInternal::initBuiltInGuiTexturesSmall(const ITextureFactory& factory, const std::vector<unsigned char>& png)
{
  (void)factory;
  
  LodePNG::Decoder pngdec;

  
  std::vector<unsigned char> dataBuffer;
  
  pngdec.decode(dataBuffer, png);
  
  const int GDW = pngdec.getWidth(); //width of the gui data
  const int GDH = pngdec.getHeight(); //height of the gui data

  /*
    Below the built in gui textures are loaded from the buffer.
    Coordinates gotten by looking at the texture that's encoded here, in a painting program.
  */
  
  //window panel
  makeTextureFromBuffer(builtInTexture[ 0], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 20, 24, 24);
  makeTextureFromBuffer(builtInTexture[ 1], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 20, 30, 24);
  makeTextureFromBuffer(builtInTexture[ 2], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 20, 36, 24);
  makeTextureFromBuffer(builtInTexture[ 3], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 26, 24, 30);
  makeTextureFromBuffer(builtInTexture[ 4], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 26, 30, 30);
  makeTextureFromBuffer(builtInTexture[ 5], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 26, 36, 30);
  makeTextureFromBuffer(builtInTexture[ 6], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 32, 24, 36);
  makeTextureFromBuffer(builtInTexture[ 7], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 32, 30, 36);
  makeTextureFromBuffer(builtInTexture[ 8], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 32, 36, 36);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.windowTextures[i] = builtInTexture[i];
  
  //button normal
  makeTextureFromBuffer(builtInTexture[ 9], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 60, 24, 64);
  makeTextureFromBuffer(builtInTexture[10], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 60, 30, 64);
  makeTextureFromBuffer(builtInTexture[11], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 60, 36, 64);
  makeTextureFromBuffer(builtInTexture[12], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 66, 24, 70);
  makeTextureFromBuffer(builtInTexture[13], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 66, 30, 70);
  makeTextureFromBuffer(builtInTexture[14], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 66, 36, 70);
  makeTextureFromBuffer(builtInTexture[15], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 72, 24, 76);
  makeTextureFromBuffer(builtInTexture[16], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 72, 30, 76);
  makeTextureFromBuffer(builtInTexture[17], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 72, 36, 76);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonTextures[i] = builtInTexture[i + 9];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonOverTextures[i] = builtInTexture[i + 9];
  
  //button pressed
  makeTextureFromBuffer(builtInTexture[18], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 60, 44, 64);
  makeTextureFromBuffer(builtInTexture[19], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 60, 50, 64);
  makeTextureFromBuffer(builtInTexture[20], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 60, 56, 64);
  makeTextureFromBuffer(builtInTexture[21], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 66, 44, 70);
  makeTextureFromBuffer(builtInTexture[22], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 66, 50, 70);
  makeTextureFromBuffer(builtInTexture[23], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 66, 56, 70);
  makeTextureFromBuffer(builtInTexture[24], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 72, 44, 76);
  makeTextureFromBuffer(builtInTexture[25], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 72, 50, 76);
  makeTextureFromBuffer(builtInTexture[26], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 72, 56, 76);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.buttonDownTextures[i] = builtInTexture[i + 18];
  
  //button arrow up
  makeTextureFromBuffer(builtInTexture[27], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 100, 76, 116);
  builtInGuiSet.arrowN = builtInTexture[27];
  //button arrow down
  makeTextureFromBuffer(builtInTexture[28], &dataBuffer[0], GDW, GDH, AE_PinkKey, 100, 100, 116, 116);
  builtInGuiSet.arrowS = builtInTexture[28];
  //button arrow left
  makeTextureFromBuffer(builtInTexture[29], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 80, 76, 96);
  builtInGuiSet.arrowW = builtInTexture[29];
  //button arrow right
  makeTextureFromBuffer(builtInTexture[30], &dataBuffer[0], GDW, GDH, AE_PinkKey, 100, 80, 116, 96);
  builtInGuiSet.arrowE = builtInTexture[30];
  
  //scroller
  makeTextureFromBuffer(builtInTexture[31], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 80, 56, 96);
  builtInGuiSet.emptyButton = builtInTexture[31];
  builtInGuiSet.scroller = builtInTexture[31];
  //scrollbar background
  makeTextureFromBuffer(builtInTexture[32], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 80, 36, 96);
  builtInGuiSet.scrollbarBackground = builtInTexture[32];
  builtInGuiSet.scrollBarPairCorner = builtInTexture[32];
  
  //check box unchecked
  makeTextureFromBuffer(builtInTexture[33], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 180, 36, 196);
  builtInGuiSet.checkBox[0] = builtInTexture[33];
  //check box checked
  makeTextureFromBuffer(builtInTexture[34], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 180, 76, 196);
  builtInGuiSet.checkBox[1] = builtInTexture[34];
  
  //bullet unchecked
  makeTextureFromBuffer(builtInTexture[35], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 220, 36, 236);
  builtInGuiSet.bullet[0] = builtInTexture[35];
  //bullet checked
  makeTextureFromBuffer(builtInTexture[36], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 220, 76, 236);
  builtInGuiSet.bullet[1] = builtInTexture[36];
  
  //horizontal line
  makeTextureFromBuffer(builtInTexture[41], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 140, 24, 144);
  makeTextureFromBuffer(builtInTexture[42], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 140, 30, 144);
  makeTextureFromBuffer(builtInTexture[43], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 140, 36, 144);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.hline[i] = builtInTexture[i + 41];
  
  //vertical line
  makeTextureFromBuffer(builtInTexture[44], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 140, 64, 144);
  makeTextureFromBuffer(builtInTexture[45], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 146, 64, 150);
  makeTextureFromBuffer(builtInTexture[46], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 152, 64, 156);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.vline[i] = builtInTexture[i + 44];
  
  //window top bar (also a horizontal line)
  makeTextureFromBuffer(builtInTexture[47], &dataBuffer[0], GDW, GDH, AE_PinkKey, 100, 20, 104, 36);
  makeTextureFromBuffer(builtInTexture[48], &dataBuffer[0], GDW, GDH, AE_PinkKey, 106, 20, 110, 36);
  makeTextureFromBuffer(builtInTexture[49], &dataBuffer[0], GDW, GDH, AE_PinkKey, 112, 20, 116, 36);
  for(size_t i = 0; i < 3; i++) builtInGuiSet.windowTop[i] = builtInTexture[i + 47];
  
  //round button
  makeTextureFromBuffer(builtInTexture[51], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 140, 56, 156);
  builtInGuiSet.roundButton = builtInTexture[51];
  builtInGuiSet.slider = builtInTexture[51];
  
  //close button
  makeTextureFromBuffer(builtInTexture[54], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 40, 36, 56);
  builtInGuiSet.closeButton = builtInTexture[54];

  //minimize button
  //55
  
  //maximize button
  //56
  
  //????
  //57
  
  //resizer of window (corner at bottom right)
  makeTextureFromBuffer(builtInTexture[58], &dataBuffer[0], GDW, GDH, AE_PinkKey, 140, 20, 156, 36);
  builtInGuiSet.resizer = builtInTexture[58];
  
  //tab unselected
  makeTextureFromBuffer(builtInTexture[60], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 260, 24, 264);
  makeTextureFromBuffer(builtInTexture[61], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 260, 30, 264);
  makeTextureFromBuffer(builtInTexture[62], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 260, 36, 264);
  makeTextureFromBuffer(builtInTexture[63], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 266, 24, 270);
  makeTextureFromBuffer(builtInTexture[64], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 266, 30, 270);
  makeTextureFromBuffer(builtInTexture[65], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 266, 36, 270);
  makeTextureFromBuffer(builtInTexture[66], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 272, 24, 276);
  makeTextureFromBuffer(builtInTexture[67], &dataBuffer[0], GDW, GDH, AE_PinkKey, 26, 272, 30, 276);
  makeTextureFromBuffer(builtInTexture[68], &dataBuffer[0], GDW, GDH, AE_PinkKey, 32, 272, 36, 276);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabUnSelected[i] = builtInTexture[i + 60];
  
  //tab selected
  makeTextureFromBuffer(builtInTexture[69], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 260, 44, 264);
  makeTextureFromBuffer(builtInTexture[70], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 260, 50, 264);
  makeTextureFromBuffer(builtInTexture[71], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 260, 56, 264);
  makeTextureFromBuffer(builtInTexture[72], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 266, 44, 270);
  makeTextureFromBuffer(builtInTexture[73], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 266, 50, 270);
  makeTextureFromBuffer(builtInTexture[74], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 266, 56, 270);
  makeTextureFromBuffer(builtInTexture[75], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 272, 44, 276);
  makeTextureFromBuffer(builtInTexture[76], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 272, 50, 276);
  makeTextureFromBuffer(builtInTexture[77], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 272, 56, 276);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.tabSelected[i] = builtInTexture[i + 69];
  
  //small check box unchecked
  makeTextureFromBuffer(builtInTexture[78], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 200, 31, 211);
  builtInGuiSet.smallCheckBox[0] = builtInTexture[78];
  //small check box checked
  makeTextureFromBuffer(builtInTexture[79], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 200, 71, 211);
  builtInGuiSet.smallCheckBox[1] = builtInTexture[79];
  
  //horizontal small slider button
  makeTextureFromBuffer(builtInTexture[80], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 160, 45, 171);
  builtInGuiSet.smallSliderH = builtInTexture[80];
  
  //border panel
  makeTextureFromBuffer(builtInTexture[81], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 280, 44, 284);
  makeTextureFromBuffer(builtInTexture[82], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 280, 50, 284);
  makeTextureFromBuffer(builtInTexture[83], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 280, 56, 284);
  makeTextureFromBuffer(builtInTexture[84], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 286, 44, 290);
  makeTextureFromBuffer(builtInTexture[85], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 286, 50, 290);
  makeTextureFromBuffer(builtInTexture[86], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 286, 56, 290);
  makeTextureFromBuffer(builtInTexture[87], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 292, 44, 296);
  makeTextureFromBuffer(builtInTexture[88], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 292, 50, 296);
  makeTextureFromBuffer(builtInTexture[89], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 292, 56, 296);
  for(size_t i = 0; i < 9; i++) builtInGuiSet.borderPanelTextures[i] = builtInTexture[i + 81];

  //fgbgcolor arrows
  makeTextureFromBuffer(builtInTexture[99], &dataBuffer[0], GDW, GDH, AE_PinkKey, 20, 280, 36, 296);
  builtInGuiSet.fgBgColorArrows = builtInTexture[99];
  
  //invisible button panel
  makeTextureFromBuffer(builtInTexture[118], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 280, 44, 284);
  makeTextureFromBuffer(builtInTexture[119], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 280, 50, 284);
  makeTextureFromBuffer(builtInTexture[120], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 280, 56, 284);
  makeTextureFromBuffer(builtInTexture[121], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 286, 44, 290);
  makeTextureFromBuffer(builtInTexture[122], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 286, 50, 290);
  makeTextureFromBuffer(builtInTexture[123], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 286, 56, 290);
  makeTextureFromBuffer(builtInTexture[124], &dataBuffer[0], GDW, GDH, AE_PinkKey, 40, 292, 44, 296);
  makeTextureFromBuffer(builtInTexture[125], &dataBuffer[0], GDW, GDH, AE_PinkKey, 46, 292, 50, 296);
  makeTextureFromBuffer(builtInTexture[126], &dataBuffer[0], GDW, GDH, AE_PinkKey, 52, 292, 56, 296);  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonTextures[i] = builtInTexture[i + 118];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonOverTextures[i] = builtInTexture[i + 118];
  
  //invisible button panel pressed
  makeTextureFromBuffer(builtInTexture[127], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 280, 64, 284);
  makeTextureFromBuffer(builtInTexture[128], &dataBuffer[0], GDW, GDH, AE_PinkKey, 66, 280, 70, 284);
  makeTextureFromBuffer(builtInTexture[129], &dataBuffer[0], GDW, GDH, AE_PinkKey, 72, 280, 76, 284);
  makeTextureFromBuffer(builtInTexture[130], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 286, 64, 290);
  makeTextureFromBuffer(builtInTexture[131], &dataBuffer[0], GDW, GDH, AE_PinkKey, 66, 286, 70, 290);
  makeTextureFromBuffer(builtInTexture[132], &dataBuffer[0], GDW, GDH, AE_PinkKey, 72, 286, 76, 290);
  makeTextureFromBuffer(builtInTexture[133], &dataBuffer[0], GDW, GDH, AE_PinkKey, 60, 292, 64, 296);
  makeTextureFromBuffer(builtInTexture[134], &dataBuffer[0], GDW, GDH, AE_PinkKey, 66, 292, 70, 296);
  makeTextureFromBuffer(builtInTexture[135], &dataBuffer[0], GDW, GDH, AE_PinkKey, 72, 292, 76, 296);  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonTextures[i] = builtInTexture[i + 118];
  for(size_t i = 0; i < 9; i++) builtInGuiSet.invisibleButtonDownTextures[i] = builtInTexture[i + 127];

  
  //panels
  builtInPanel[0].makeTextured(&builtInGuiSet.windowTextures[0]/*, RGB_White*/);
  builtInGuiSet.windowPanel = &builtInPanel[0];
  builtInPanel[1].makeTextured(&builtInGuiSet.buttonTextures[0]/*, RGB_White*/);
  builtInGuiSet.buttonPanel = &builtInPanel[1];
  builtInPanel[2].makeTextured(&builtInGuiSet.buttonOverTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.buttonOverPanel = &builtInPanel[2];
  builtInPanel[3].makeTextured(&builtInGuiSet.buttonDownTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.buttonDownPanel = &builtInPanel[3];
  builtInPanel[4].makeTextured(&builtInGuiSet.tabUnSelected[0]/*, RGB_Grey*/);
  builtInGuiSet.tabUnSelectedPanel = &builtInPanel[4];
  builtInPanel[5].makeTextured(&builtInGuiSet.tabSelected[0]/*, RGB_Grey*/);
  builtInGuiSet.tabSelectedPanel = &builtInPanel[5];
  builtInPanel[6].makeTextured(&builtInGuiSet.borderPanelTextures[0]/*, RGB_White*/);
  builtInGuiSet.borderPanel = &builtInPanel[6];
  //7, 8, 9, 10??? who cares this number system will be refactored away soon anyway
  builtInPanel[11].makeTextured(&builtInGuiSet.invisibleButtonTextures[0]/*, RGB_White*/);
  builtInGuiSet.invisibleButtonPanel = &builtInPanel[11];
  builtInPanel[12].makeTextured(&builtInGuiSet.invisibleButtonOverTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.invisibleButtonOverPanel = &builtInPanel[12];
  builtInPanel[13].makeTextured(&builtInGuiSet.invisibleButtonDownTextures[0]/*, RGB_Grey*/);
  builtInGuiSet.invisibleButtonDownPanel = &builtInPanel[13];

  //rules (= 1D versions of panels)
  builtInRule[0].makeHorizontal(builtInTexture[41], builtInTexture[42], builtInTexture[43]);
  builtInGuiSet.sliderHRule = &builtInRule[0];
  builtInRule[1].makeVertical(builtInTexture[44], builtInTexture[45], builtInTexture[46]);
  builtInGuiSet.sliderVRule = &builtInRule[1];
  builtInGuiSet.smallSliderHRule = &builtInRule[0]; //TODO: give it its own rule, which uses the other texture in the GUI texture set
  builtInRule[2].makeHorizontal(builtInTexture[47], builtInTexture[48], builtInTexture[49]);
  builtInGuiSet.windowTopRule = &builtInRule[2];
  
  //colors
  builtInGuiSet.mainColor = RGB_White;
  builtInGuiSet.mouseOverColor = RGB_Brightred;
  builtInGuiSet.mouseDownColor = RGB_Grey;
  
  //font
  builtInGuiSet.panelButtonFont[0].color = RGB_Black;
  builtInGuiSet.panelButtonFont[1].color = RGB_Black;
  builtInGuiSet.panelButtonFont[2].color = RGB_Black;
  builtInGuiSet.textButtonFont[0].color = RGB_White;
  builtInGuiSet.textButtonFont[0].shadow = true;
  builtInGuiSet.textButtonFont[0].shadowColor = RGB_Black;
  builtInGuiSet.textButtonFont[1].color = RGB_Red;
  builtInGuiSet.textButtonFont[1].shadow = true;
  builtInGuiSet.textButtonFont[1].shadowColor = RGB_Black;
  builtInGuiSet.textButtonFont[2].color = RGB_Red;
  builtInGuiSet.textButtonFont[2].shadow = true;
  builtInGuiSet.textButtonFont[2].shadowColor = RGB_Black;
  builtInGuiSet.windowTopFont.color = RGB_White;
  builtInGuiSet.windowTopFont.shadow = true;
  builtInGuiSet.windowTopFont.shadowColor = RGB_Black;
  builtInGuiSet.tooltipFont = Font("lpi6", RGB_Black);
  
  //menu panels and separators
  builtInGuiSet.horMenuPanel = builtInGuiSet.buttonPanel;
  builtInGuiSet.verMenuPanel = builtInGuiSet.buttonPanel;
  builtInGuiSet.horMenuSeparator = builtInGuiSet.sliderVRule;
  builtInGuiSet.verMenuSeparator = builtInGuiSet.sliderHRule;
}

////////////////////////////////////////////////////////////////////////////////
//BACKPANEL/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
The BackPanel functions.
BackPanel is a class containing 9 textures, being able to draw textured panels of any size with those.
See the .h file for meaning of vareous variables of the classes
*/

//Constructors
BackPanel::BackPanel()
{
  this->enableSides = 0;
  this->enableCenter = 0;
  this->t00 = emptyTexture;
  this->t01 = emptyTexture;
  this->t02 = emptyTexture;
  this->t10 = emptyTexture;
  this->t11 = emptyTexture;
  this->t12 = emptyTexture;
  this->t20 = emptyTexture;
  this->t21 = emptyTexture;
  this->t22 = emptyTexture;
}


void BackPanel::makeUntextured()
{
  this->enableSides = 0;
  this->enableCenter = 0;
  this->t00 = emptyTexture;
  this->t01 = emptyTexture;
  this->t02 = emptyTexture;
  this->t10 = emptyTexture;
  this->t11 = emptyTexture;
  this->t12 = emptyTexture;
  this->t20 = emptyTexture;
  this->t21 = emptyTexture;
  this->t22 = emptyTexture;
}

void BackPanel::makeTextured9(const ITexture* t00, const ITexture* t01, const ITexture* t02, const ITexture* t10, const ITexture* t11, const ITexture* t12, const ITexture* t20, const ITexture* t21, const ITexture* t22)
{
  this->enableSides = 1;
  this->enableCenter = 1;
  this->t00 = t00;
  this->t01 = t01;
  this->t02 = t02;
  this->t10 = t10;
  this->t11 = t11;
  this->t12 = t12;
  this->t20 = t20;
  this->t21 = t21;
  this->t22 = t22;
}

void BackPanel::makeTextured(const ITexture** t00)
{
  this->enableSides = 1;
  this->enableCenter = 1;
  this->t00 = t00[0];
  this->t01 = t00[1];
  this->t02 = t00[2];
  this->t10 = t00[3];
  this->t11 = t00[4];
  this->t12 = t00[5];
  this->t20 = t00[6];
  this->t21 = t00[7];
  this->t22 = t00[8];
}

/*The draw function of the BackPanel: draws the 9 different textures tiled or
non-tiled at the positions needed to get a window at the correct position
with the correct size*/
void BackPanel::draw(IDrawer2D& drawer, int x, int y, int width, int height, const ColorRGB& colorMod) const
{
  //draw rectangle  
  if(!enableCenter)
  {
    drawer.drawRectangle(x, y, x + width, y + height, colorMod, true);
  }
  //if center texture enabled, draw center texture instead, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
  else
  {
    if(t11 != 0) drawer.drawTextureRepeated(t11, x + t00->getU(), y + t00->getV(), x + width - t02->getU(), y + height - t20->getV(), colorMod);
  }
  
  //draw sides
  if(enableSides)
  {
    drawer.drawTextureRepeated(t01, x + t00->getU(),         y                       , x + width - t02->getU(), y + t00->getV()         , colorMod);
    drawer.drawTextureRepeated(t21, x + t00->getU(),         y + height - t20->getV(), x + width - t02->getU(), y + height              , colorMod);
    drawer.drawTextureRepeated(t10, x,                       y + t00->getV()         , x + t00->getU()        , y + height - t20->getV(), colorMod);
    drawer.drawTextureRepeated(t12, x + width - t02->getU(), y + t00->getV()         , x + width              , y + height - t20->getV(), colorMod);
  }
    
  //draw corners
  if(enableSides)
  {
    drawer.drawTexture(t00, x                      , y                       , colorMod); //top left 
    drawer.drawTexture(t02, x + width - t02->getU(), y                       , colorMod); //top right
    drawer.drawTexture(t20, x                      , y + height - t20->getV(), colorMod); //bottom left
    drawer.drawTexture(t22, x + width - t02->getU(), y + height - t20->getV(), colorMod); //bottom right
  }
}

////////////////////////////////////////////////////////////////////////////////
//BACKRULE//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
a BackRule is a collection of 3 textures (2 corners and 1 tileable center) that
can form a horizontal or vertical line
*/

//Constructors
BackRule::BackRule()
{
  this->enableSides = 0;
  this->t0 = emptyTexture;
  this->t1 = emptyTexture;
  this->t2 = emptyTexture;
}


void BackRule::makeHorizontal(const ITexture* t0, ITexture* t1, ITexture* t2)
{
  this->direction = H;
  this->enableSides = 1;
  this->t0 = t0;
  this->t1 = t1;
  this->t2 = t2;
}

void BackRule::makeHorizontal1(const ITexture* t0)
{
  this->direction = H;
  this->enableSides = 1;
  this->t0 = &t0[0];
  this->t1 = &t0[1];
  this->t2 = &t0[2];
}

void BackRule::makeVertical(const ITexture* t0, ITexture* t1, ITexture* t2)
{
  this->direction = V;
  this->enableSides = 1;
  this->t0 = t0;
  this->t1 = t1;
  this->t2 = t2;
}

void BackRule::makeVertical1(const ITexture* t0)
{
  this->direction = V;
  this->enableSides = 1;
  this->t0 = &t0[0];
  this->t1 = &t0[1];
  this->t2 = &t0[2];
}

void BackRule::draw(IDrawer2D& drawer, int x, int y, int length, const ColorRGB& colorMod) const
{
  if(direction == H)
  {
    //draw center texture tiled, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    drawer.drawTextureRepeated(t1, x + t0->getU(), y, x + length - t2->getU(), y + t0->getV(), colorMod);
  
    
    //draw endpoints
    if(enableSides)
    {
      drawer.drawTexture(t0, x, y, colorMod); //left
      drawer.drawTexture(t2, x + length - t2->getU(), y, colorMod); //right
    }
  }
  else
  {
    //draw center texture tiled, as repeated texture. MUST HAVE POWER OF TWO SIDES TO LOOK AS SUPPOSED!
    drawer.drawTextureRepeated(t1, x, y + t0->getV(), x + t0->getU(), y + length - t2->getV(), colorMod);
  
    
    //draw endpoints
    if(enableSides)
    {
      drawer.drawTexture(t0, x, y, colorMod); //left
      drawer.drawTexture(t2, x, y + length - t2->getV(), colorMod); //right
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

GUIPartDrawerInternal::GUIPartDrawerInternal(const ITextureFactory& factory, IDrawer2D* drawer, ITextDrawer* textdrawer)
: drawer(drawer)
, textdrawer(textdrawer)
{
  builtInTexture.resize(160);
  for(size_t i = 0; i < builtInTexture.size(); i++) builtInTexture[i] = factory.createNewTexture();
  initBuiltInGui(factory);
  guiset = &builtInGuiSet;
}

GUIPartDrawerInternal::~GUIPartDrawerInternal()
{
  for(size_t i = 0; i < builtInTexture.size(); i++) delete builtInTexture[i];
}


void GUIPartDrawerInternal::drawGUIPart(GUIPart part, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  (void)mod;
  
  switch(part)
  {
    case GP_WINDOW_PANEL:
    {
      guiset->windowPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_WINDOW_TOP:
    {
      guiset->windowTopRule->draw(*drawer, x0, y0, x1 - x0, RGB_Lightblue);
      break;
    }
    case GP_WINDOW_CLOSE:
    {
      ColorRGB color = mod.mouseover ? guiset->mouseOverColor : RGB_White;
      drawer->drawTexture(guiset->closeButton, x1 - guiset->closeButton->getU(), y0, color);
      break;
    }
    case GP_WINDOW_RESIZER:
    {
      drawer->drawTexture(guiset->resizer, x1 - guiset->resizer->getU(), y1 - guiset->resizer->getV());
      break;
    }
    case GP_BUTTON_PANEL:
    {
      if(mod.mousedown) guiset->buttonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else if(mod.mouseover) guiset->buttonOverPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else guiset->buttonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_BUTTON_PANEL_UP:
    {
      guiset->buttonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_BUTTON_PANEL_DOWN:
    {
      guiset->buttonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_INVISIBLE_BUTTON_PANEL:
    {
      if(mod.mousedown) guiset->invisibleButtonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else if(mod.mouseover) guiset->invisibleButtonOverPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      else guiset->invisibleButtonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_INVISIBLE_BUTTON_PANEL_UP:
    {
      guiset->invisibleButtonPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_INVISIBLE_BUTTON_PANEL_DOWN:
    {
      guiset->invisibleButtonDownPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_PANEL_BORDER:
    {
      guiset->borderPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_TAB_UNSELECTED:
    {
      guiset->tabUnSelectedPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_TAB_SELECTED:
    {
      guiset->tabSelectedPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_CHECKBOX_OFF:
    {
      if(x1 - x0 >= 16) drawer->drawTexture(guiset->checkBox[0], x0, y0);
      else drawer->drawTexture(guiset->smallCheckBox[0], x0, y0);
      break;
    }
    case GP_CHECKBOX_ON:
    {
      if(x1 - x0 >= 16) drawer->drawTexture(guiset->checkBox[1], x0, y0);
      else drawer->drawTexture(guiset->smallCheckBox[1], x0, y0);
      break;
    }
    case GP_SLIDER_HBUTTON:
    {
      drawer->drawTexture(guiset->slider, x0, y0);
      break;
    }
    case GP_SLIDER_VBUTTON:
    {
      drawer->drawTexture(guiset->slider, x0, y0);
      break;
    }
    case GP_SLIDER_HBACK:
    {
      int rulerCenter = guiset->sliderHRule->t0->getV() / 2;
      int centerPos = (y0 + y1) / 2;
      guiset->sliderHRule->draw(*drawer, x0, centerPos - rulerCenter, x1 - x0);
      break;
    }
    case GP_SLIDER_VBACK:
    {
      int rulerCenter = guiset->sliderVRule->t0->getU() / 2;
      int centerPos = (x0 + x1) / 2;
      guiset->sliderVRule->draw(*drawer, centerPos - rulerCenter, y0, y1 - y0);
      break;
    }
    case GP_SMALL_SLIDER_HBUTTON:
    {
      drawer->drawTexture(guiset->smallSliderH, x0, y0);
      break;
    }
    case GP_SMALL_SLIDER_VBUTTON:
    {
      //TODO: graphic image doesn't exist yet.
      break;
    }
 
    case GP_SMALL_SLIDER_HBACK:
    {
      int rulerCenter = guiset->sliderHRule->t0->getV() / 2;
      int centerPos = (y0 + y1) / 2;
      guiset->sliderHRule->draw(*drawer, x0, centerPos - rulerCenter, x1 - x0);
      break;
    }
    case GP_SMALL_SLIDER_VBACK:
    {
      int rulerCenter = guiset->sliderVRule->t0->getU() / 2;
      int centerPos = (x0 + x1) / 2;
      guiset->sliderVRule->draw(*drawer, centerPos - rulerCenter, y0, y1 - y0);
      break;
    }
    case GP_SCROLLBAR_N:
    {
      drawer->drawTexture(guiset->arrowN, x0, y0);
      break;
    }
    case GP_SCROLLBAR_E:
    {
      drawer->drawTexture(guiset->arrowE, x0, y0);
      break;
    }
    case GP_SCROLLBAR_S:
    {
      drawer->drawTexture(guiset->arrowS, x0, y0);
      break;
    }
    case GP_SCROLLBAR_W:
    {
      drawer->drawTexture(guiset->arrowW, x0, y0);
      break;
    }
    case GP_SCROLLBAR_SCROLLER:
    {
      drawer->drawTexture(guiset->scroller, x0, y0);
      break;
    }
    case GP_SCROLLBAR_HBACK:
    {
      drawer->drawTextureRepeated(guiset->scrollbarBackground, x0, y0, x1, y1);
      break;
    }
    case GP_SCROLLBAR_VBACK:
    {
      drawer->drawTextureRepeated(guiset->scrollbarBackground, x0, y0, x1, y1);
      break;
    }
    case GP_SCROLLBARPAIR_CORNER:
    {
      const ITexture* t = guiset->scrollBarPairCorner;
      drawer->drawTexture(t, x1 - t->getU(), y1 - t->getV());
      break;
    }
    case GP_TOOLTIP_BACKGROUND:
    {
      drawer->drawRectangle(x0, y0, x1, y1, RGBA_Lightyellow(224), true);
      drawer->drawRectangle(x0, y0, x1, y1, RGB_Black, false);
      break;
    }
    case GP_FG_BG_COLOR_ARROWS:
    {
      drawer->drawTexture(guiset->fgBgColorArrows, x1 - guiset->fgBgColorArrows->getU(), y0);
      break;
    }
    case GP_HMENU_PANEL:
    {
      guiset->horMenuPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_VMENU_PANEL:
    {
      guiset->verMenuPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0);
      break;
    }
    case GP_VMENU_SEPARATOR:
    {
      int rulerCenter = guiset->verMenuSeparator->t0->getV() / 2;
      int centerPos = (y0 + y1) / 2;
      guiset->verMenuSeparator->draw(*drawer, x0, centerPos - rulerCenter, x1 - x0);
      break;
    }
    case GP_HMENU_SEPARATOR:
    {
      int rulerCenter = guiset->horMenuSeparator->t0->getU() / 2;
      int centerPos = (x0 + x1) / 2;
      guiset->horMenuSeparator->draw(*drawer, centerPos - rulerCenter, y0, y1 - y0);
      break;
    }

    default:
      return;
  }
}

void GUIPartDrawerInternal::drawGUIPartColor(GUIPart part, const ColorRGB& color, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  (void)mod;
  
  switch(part)
  {
    case GPC_WINDOW_PANEL:
    {
      guiset->windowPanel->draw(*drawer, x0, y0, x1 - x0, y1 - y0, color);
      break;
    }
    default:
      return;
  }
}

void GUIPartDrawerInternal::drawGUIPartText(GUIPart part, const std::string& text, int x0, int y0, int x1, int y1, const GUIPartMod& mod)
{
  (void)mod;
  
  switch(part)
  {
    case GPT_TEXT_BUTTON_TEXT:
    {
      if(mod.mousedown) textdrawer->drawText(text, (x0+x1)/2 + 2, (y0+y1)/2 + 2, guiset->textButtonFont[2], TextAlign(HA_CENTER, VA_CENTER));
      else if(mod.mouseover) textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->textButtonFont[1], TextAlign(HA_CENTER, VA_CENTER));
      else textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->textButtonFont[0], TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_PANEL_BUTTON_TEXT:
    {
      if(mod.mousedown) textdrawer->drawText(text, (x0+x1)/2 + 1, (y0+y1)/2 + 1, guiset->panelButtonFont[2], TextAlign(HA_CENTER, VA_CENTER));
      else if(mod.mouseover) textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->panelButtonFont[1], TextAlign(HA_CENTER, VA_CENTER));
      else textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->panelButtonFont[0], TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_WINDOW_TITLE:
    {
      textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, guiset->windowTopFont, TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_TOOLTIP:
    {
      x0 += 10; //shift a bit to right to avoid having mouse cursor in front of the tip
      int w, h;
      textdrawer->calcTextRectSize(w, h, text, guiset->tooltipFont);
      drawGUIPart(GP_TOOLTIP_BACKGROUND, x0 - 2, y0 - 2, x0 + w + 2, y0 + h + 2, mod);
      textdrawer->drawText(text, x0, y0, guiset->tooltipFont);
      break;
    }
    case GPT_HMENU_TEXT:
    {
      textdrawer->drawText(text, (x0+x1)/2, (y0+y1)/2, FONT_Black, TextAlign(HA_CENTER, VA_CENTER));
      break;
    }
    case GPT_VMENU_TEXT:
    {
      textdrawer->drawText(text, x0+4, (y0+y1)/2, FONT_Black, TextAlign(HA_LEFT, VA_CENTER));
      break;
    }
    default:
      return;
  }
}

size_t GUIPartDrawerInternal::getGUIPartSizeX(GUIPart part) const
{
  switch(part)
  {
    case GP_WINDOW_CLOSE: return guiset->closeButton->getU();
    case GP_WINDOW_RESIZER: return guiset->resizer->getU();
    case GP_SLIDER_HBUTTON: return guiset->slider->getU();
    case GP_SLIDER_VBUTTON: return guiset->slider->getU();
    case GP_SMALL_SLIDER_HBUTTON: return guiset->smallSliderH->getU();
    //case GP_SMALL_SLIDER_VBUTTON: return guiset->smallSliderV->getU();
    case GP_SLIDER_HBACK: return guiset->sliderHRule->t0->getU();
    case GP_SLIDER_VBACK: return guiset->sliderVRule->t0->getU();
    case GP_SMALL_SLIDER_HBACK: return guiset->sliderHRule->t0->getU();
    case GP_SMALL_SLIDER_VBACK: return guiset->sliderVRule->t0->getU();
    case GP_SCROLLBAR_N: return guiset->arrowN->getU();
    case GP_SCROLLBAR_E: return guiset->arrowE->getU();
    case GP_SCROLLBAR_S: return guiset->arrowS->getU();
    case GP_SCROLLBAR_W: return guiset->arrowW->getU();
    case GP_SCROLLBAR_SCROLLER: return guiset->scroller->getU();
    case GP_SCROLLBAR_HBACK: return guiset->scrollbarBackground->getU();
    case GP_SCROLLBAR_VBACK: return guiset->scrollbarBackground->getU();
    case GP_SCROLLBARPAIR_CORNER: return guiset->scrollBarPairCorner->getU();
    case GP_FG_BG_COLOR_ARROWS: return guiset->fgBgColorArrows->getU();
    default: return 0;
  }
}

size_t GUIPartDrawerInternal::getGUIPartSizeY(GUIPart part) const
{
  switch(part)
  {
    case GP_WINDOW_TOP: return guiset->windowTopRule->t0->getV();
    case GP_WINDOW_CLOSE: return guiset->closeButton->getV();
    case GP_WINDOW_RESIZER: return guiset->resizer->getV();
    case GP_SLIDER_HBUTTON: return guiset->slider->getV();
    case GP_SLIDER_VBUTTON: return guiset->slider->getV();
    case GP_SMALL_SLIDER_HBUTTON: return guiset->smallSliderH->getV();
    //case GP_SMALL_SLIDER_VBUTTON: return guiset->smallSliderV->getV();
    case GP_SLIDER_HBACK: return guiset->sliderHRule->t0->getV();
    case GP_SLIDER_VBACK: return guiset->sliderVRule->t0->getV();
    case GP_SMALL_SLIDER_HBACK: return guiset->sliderHRule->t0->getV();
    case GP_SMALL_SLIDER_VBACK: return guiset->sliderVRule->t0->getV();
    case GP_SCROLLBAR_N: return guiset->arrowN->getV();
    case GP_SCROLLBAR_E: return guiset->arrowE->getV();
    case GP_SCROLLBAR_S: return guiset->arrowS->getV();
    case GP_SCROLLBAR_W: return guiset->arrowW->getV();
    case GP_SCROLLBAR_SCROLLER: return guiset->scroller->getV();
    case GP_SCROLLBAR_HBACK: return guiset->scrollbarBackground->getV();
    case GP_SCROLLBAR_VBACK: return guiset->scrollbarBackground->getV();
    case GP_SCROLLBARPAIR_CORNER: return guiset->scrollBarPairCorner->getV();
    case GP_FG_BG_COLOR_ARROWS: return guiset->fgBgColorArrows->getV();
    default: return 0;
  }
}

} // namespace gui
} // namespace lpi
