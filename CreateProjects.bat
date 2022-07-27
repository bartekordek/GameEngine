set CONFIG_TYPE=%1
set OUTPUT_DIR="Build-Windows"

echo CONFIG TYPE: %CONFIG_TYPE%

if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

cmake -S . -B %OUTPUT_DIR% -DCMAKE_BUILD_TYPE=%CONFIG_TYPE%