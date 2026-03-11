Dodanie całego **C/C++ Extension Pack** to doskonały pomysł – to paczka, która zawiera w sobie od razu podświetlanie składni, motywy i narzędzia do CMake.

Przygotowałem dla Ciebie zaktualizowany, gotowy poradnik. Zrobiłem w nim jedną drobną, ale ważną zmianę: **usunąłem `twxs.cmake**`, ponieważ rozmawialiśmy wcześniej, że powoduje konflikty z oficjalną wtyczką Microsoftu, a dodałem ID paczki Extension Pack (`ms-vscode.cpptools-extension-pack`).

Na końcu instrukcji napisałem Ci, jak to najwygodniej zapisać, żeby nie stracić tego ładnego formatowania!

---

### Twój osobisty poradnik: STM32 + Docker w VS Code

#### Faza 1: Generowanie bazy (STM32CubeMX)

1. Otwórz **STM32CubeMX** na Linuxie.
2. Wybierz swój mikrokontroler i skonfiguruj piny/zegary.
3. W zakładce **Project Manager**:
* Zmień **Toolchain / IDE** na **CMake**.
* Wygeneruj kod.



#### Faza 2: Konteneryzacja (Konfiguracja VS Code)

W wygenerowanym folderze projektu stwórz ukryty folder `.devcontainer`, a w nim dwa poniższe pliki:

**1. Plik: `.devcontainer/Dockerfile**`

```dockerfile
FROM ubuntu:24.04

# Wyłączenie interaktywnych zapytań podczas instalacji
ENV DEBIAN_FRONTEND=noninteractive

# Instalacja niezbędnych narzędzi dla STM32
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    gdb-multiarch \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    stlink-tools \
    openocd \
    usbutils \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

```

**2. Plik: `.devcontainer/devcontainer.json**`

```json
{
    "name": "STM32 CMake Toolchain",
    "build": {
        "dockerfile": "Dockerfile"
    },
    // Przekazanie portów USB z Linuxa do Dockera (dla ST-Linka)
    "runArgs": [
        "--privileged",
        "-v", "/dev/bus/usb:/dev/bus/usb"
    ],
    "customizations": {
        "vscode": {
            // Te rozszerzenia zainstalują się automatycznie wewnątrz kontenera
            "extensions": [
                "ms-vscode.cpptools-extension-pack",
                "ms-vscode.cmake-tools",
                "marus25.cortex-debug"
            ],
            "settings": {
                "cmake.configureOnOpen": true,
                "cmake.generator": "Ninja"
            }
        }
    },
    "remoteUser": "root"
}

```

Po stworzeniu tych plików wciśnij **F1**, wpisz i wybierz: **`Dev Containers: Reopen in Container`**.

#### Faza 3: Budowanie i Debugowanie

Gdy środowisko się uruchomi w kontenerze:

1. **Konfiguracja CMake:**
* Wciśnij **F1** -> **`CMake: Select Configure Preset`** (wybierz np. `Debug`).
* Wciśnij **F1** -> **`CMake: Configure`**.


2. **Kompilacja:**
* Wciśnij **F7**, aby wygenerować plik `.elf`.


3. **Konfiguracja Debuggera:**
* Stwórz plik `.vscode/launch.json` i wklej poniższy kod. Pamiętaj tylko o zmianie pliku `.cfg` w linijce `target/` na właściwy dla Twojej płytki!



**3. Plik: `.vscode/launch.json**`

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Cortex Debug (OpenOCD)",
            "cwd": "${workspaceFolder}",
            "executable": "${command:cmake.launchTargetPath}",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "openocd",
            "gdbPath": "gdb-multiarch",
            "configFiles": [
                "interface/stlink.cfg",
                "target/stm32f4x.cfg" 
            ],
            "runToEntryPoint": "main",
            "showDevDebugOutput": "none",
            "preLaunchTask": "CMake: build"
        }
    ]
}

```

Wciśnij **F5** i gotowe – kod ląduje na płytce!

---

