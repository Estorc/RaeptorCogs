Installation
============

In this section, we will guide you through the installation process of RæptorCogs on your system.
We will cover the prerequisites, downloading the framework, and setting up your development environment.

Prerequisites
-------------

Before installing RæptorCogs, ensure that you have the following prerequisites installed on your system:

- A C++17 compatible compiler (e.g., GCC, Clang, MSVC)
    - For Windows, we recommend using Clang CL from LLVM or MSVC via Visual Studio.
    - For macOS, you can use Clang via Xcode Command Line Tools.
    - For Linux, GCC or Clang version 7 or higher is recommended.
- CMake version 3.15 or higher with Ninja build system
- Git for cloning the repository

Downloading RæptorCogs
----------------------
This subsection is intended to both check that RæptorCogs works on your operating system and to
allow you to contribute to its development if you wish to do so.
If you just want to use RæptorCogs in your own projects and are confident in your setup, you can
skip this part and go directly to the :ref:`setup-working-environment` section.

Testing operating system compatibility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can download RæptorCogs by cloning the official GitHub repository. Open your terminal and run the
following command:

.. code-block:: console

   $ git clone https://github.com/Estorc/RaeptorCogs.git

1. Navigate to the cloned RæptorCogs directory:

.. code-block:: console

   $ cd RaeptorCogs

2. Create a build directory and navigate into it:

.. code-block:: console

   $ mkdir build && cd build

3. Run CMake to configure the project:

.. code-block:: console

   $ cmake ..

4. Build the framework using your preferred build system:

.. code-block:: console

   $ cmake --build . --target RaeptorCogsDemo

And voilà! You have successfully installed RæptorCogs on your system.
If you can run the demo application, this means that everything is working correctly on your setup.

Contributing to RæptorCogs
~~~~~~~~~~~~~~~~~~~~~~~~~~

If you wish to contribute to the development of RæptorCogs, you can fork the repository and
create a pull request with your changes. Make sure to follow the coding standards and guidelines
provided in the repository.

.. _setup-working-environment:
Setup a working environment
---------------------------
Testing operating system compatibility is funny enough, but to develop your own applications, you
need to set up a working environment.
If you're not familiar with CMake, we recommend reading the `CMake documentation <https://cmake.org/documentation/>`_
to understand how to create CMake projects.

Here is a minimal CMakeLists.txt file to get you started with RæptorCogs:

.. code-block:: cmake
    :linenos:

    cmake_minimum_required(VERSION 3.15)
    project(YourBrandNewRaeptorApp)
    set(CMAKE_CXX_STANDARD 17)

    include(FetchContent)
    FetchContent_Declare(
        RaeptorCogs
        GIT_REPOSITORY https://github.com/Estorc/RaeptorCogs.git
    )
    FetchContent_MakeAvailable(RaeptorCogs)

    add_executable(YourBrandNewRaeptorApp your_source_files.cpp)
    target_link_libraries(YourBrandNewRaeptorApp PRIVATE RaeptorCogs::RaeptorCogs)

Replace `your_source_files.cpp` with the actual source files of your application.
With this setup, you can start developing your own applications using RæptorCogs.
Now that you have RæptorCogs installed and your development environment set up, you're ready to
create your first application! Proceed to the next section to learn how to create a simple
application that opens a window and displays graphics using RæptorCogs.