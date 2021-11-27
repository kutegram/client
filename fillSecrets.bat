cd library
echo #define DC_IP "149.154.167.50" >> apivalues.h
echo #define DC_PORT 443 >> apivalues.h
echo #define DC_NUMBER 2 >> apivalues.h
echo #define APP_ID %TG_API_ID% >> apivalues.h
echo #define APP_HASH "%TG_API_HASH%" >> apivalues.h
cd ..
