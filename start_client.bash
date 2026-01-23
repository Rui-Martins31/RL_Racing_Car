#!/bin/bash

OUTPUT="agent"

echo "============================================"
echo "  TORCS PPO Client - Build & Run"
echo "============================================"
echo ""

# ============================================
# 1. LIMPEZA
# ============================================
echo "[BUILD] Cleaning..."
rm -f "$OUTPUT"
rm -f *.o

# ============================================
# 2. COMPILAÇÃO
# ============================================
echo "[BUILD] Compiling client..."

g++ -std=c++17 -O2 -Wall -Wextra \
    client.cpp \
    control.cpp \
    utils/parser.cpp \
    -o "$OUTPUT"

if [ $? -ne 0 ]; then
    echo ""
    echo "[ERROR] Compilation failed!"
    exit 1
fi

echo "[BUILD] Compilation successful!"
echo ""

# ============================================
# 3. VERIFICAÇÃO DO PPO SERVER
# ============================================
echo "[CHECK] Verifying PPO server..."

if ! pgrep -f "ppo_server.py" > /dev/null; then
    echo "[WARNING] PPO server is NOT running!"
    echo ""
    echo "Please start it in another terminal:"
    echo "  python3 ppo_server.py"
    echo ""
    echo "Press ENTER when ready, or Ctrl+C to cancel..."
    read -r
else
    PPO_PID=$(pgrep -f "ppo_server.py")
    echo "[CHECK] PPO server is running (PID: $PPO_PID)"
fi

echo ""

# ============================================
# 4. EXECUÇÃO
# ============================================
echo "[RUN] Starting TORCS client..."
echo "============================================"
echo ""

./"$OUTPUT"

EXIT_CODE=$?

echo ""
echo "============================================"
echo "[CLIENT] Terminated (exit code: $EXIT_CODE)"
echo "============================================"

exit $EXIT_CODE