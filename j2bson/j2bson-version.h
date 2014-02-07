/*
 * Copyright 2014 Jason Choy <jjwchoy@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef J2BSON_VERSION_H
#define J2BSON_VERSION_H

#define J2BSON_MAJOR_VERSION (0)
#define J2BSON_MINOR_VERSION (1)
#define J2BSON_MICRO_VERSION (0)

#define J2BSON_VERSION (0.1.0)
#define J2BSON_VERSION_S "0.1.0"

#define J2BSON_VERSION_HEX (J2BSON_MAJOR_VERSION << 24 | \
                            J2BSON_MINOR_VERSION << 16 | \
                            J2BSON_MICRO_VERSION << 8)

#define J2BSON_CHECK_VERSION(major,minor,micro) \
    (J2BSON_MAJOR_VERSION > (major) || \
     (J2BSON_MAJOR_VERSION == (major) && J2BSON_MINOR_VERSION > (minor)) || \
     (J2BSON_MAJOR_VERSION == (major) && J2BSON_MINOR_VERSION == (minor) &&\
      J2BSON_MICRO_VERSION >= (micro)))

#endif
