## Development enviroment installation

This firmware is built with **esp-idf 4.4 version**

### Installation
Read the Espressif's [Official documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation-step-by-step) to install the enviroment

### Build and Run

* Build (compile)
    ```python
    idf.py build
    ```

* Flash firmware
    ```python
    idf.py -p <PORT> -b 460800 flash
    ```

    Here <PORT> depends of which port the operating systemas assignes to the lock

* Erase flash
    ```python
    idf.py -p <PORT> -b 460800 erase_flash
    ```

* Open serial monitor
    ```python
    idf.py -p <PORT> monitor
    ```

* Clean build files
    ```python
    idf.py full_clean
    ```