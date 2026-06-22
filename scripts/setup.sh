#!/bin/bash

echo "===================================================="
echo "        PokeEngine Environment Setup"
echo "===================================================="

# WINDOWS DETECTION

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then

    echo "Detectado: Windows"

    echo "Las dependencias del sistema no necesitan instalación."
    echo "Visual Studio incluye compilador, SDK y herramientas necesarias."

    echo "===================================================="
    echo "Setup terminado."
    echo "Ahora abre el proyecto con Visual Studio o VSCode."
    echo "===================================================="

    exit 0
fi

# LINUX DETECTION

if [[ "$OSTYPE" == "linux-gnu"* ]]; then

    echo "Detectado: Linux"


    DEPENDENCIAS=(
        "build-essential"
        "cmake"
        "ninja-build"
        "curl"
        "zip"
        "unzip"
        "tar"
        "pkg-config"
        "libx11-dev"
        "libxft-dev"
        "libxtst-dev"
        "libibus-1.0-dev"
        "libdbus-1-dev"
        "libudev-dev"
        "libxcursor-dev"
        "libxrandr-dev"
        "libxinerama-dev"
        "libxi-dev"
        "libxext-dev"
        "libxfixes-dev"
        "libwayland-dev"
        "libxkbcommon-dev"
        "libgl1-mesa-dev"
        "libegl1-mesa-dev"
        "wayland-protocols"
    )


    PAQUETES_A_INSTALAR=()


    echo "Comprobando dependencias..."


    for paquete in "${DEPENDENCIAS[@]}"; do

        if dpkg -s "$paquete" >/dev/null 2>&1; then

            echo "$paquete instalado"

        else

            echo "$paquete falta"
            PAQUETES_A_INSTALAR+=("$paquete")

        fi

    done



    if [ ${#PAQUETES_A_INSTALAR[@]} -ne 0 ]; then

        echo ""
        echo "Instalando dependencias:"
        echo "${PAQUETES_A_INSTALAR[@]}"

        sudo apt update
        sudo apt install -y "${PAQUETES_A_INSTALAR[@]}"

    else

        echo ""
        echo "Todas las dependencias están instaladas."

    fi



    echo "===================================================="
    echo "Setup Linux terminado."
    echo "Ahora puedes ejecutar:"
    echo ""
    echo "cmake --preset linux-debug"
    echo "cmake --build --preset linux-debug"
    echo "===================================================="

    exit 0

fi


echo "Sistema operativo no soportado."
exit 1