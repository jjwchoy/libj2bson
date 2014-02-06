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
