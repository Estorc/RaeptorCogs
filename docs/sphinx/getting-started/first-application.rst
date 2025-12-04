First application
=================
This guide takes you through writing a small application using RæptorCogs.
By the end of this tutorial, you will have a basic understanding of how to create a window
and render simple graphics using the framework.


.. cpp:namespace:: RaeptorCogs

Step by step
------------

Including RæptorCogs headers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
To start using RæptorCogs in your application, you need to include the necessary header.

.. code-block:: cpp

   #include <RaeptorCogs/RaeptorCogs.hpp>

This header includes all the core functionalities of RæptorCogs. It allow you to manipulate
the state of the framework, create windows, render graphics, and handle input events.

When using components like Sprite or Text, you may need to include their specific headers as well:

.. code-block:: cpp

   #include <RaeptorCogs/Graphics/Sprite.hpp>
   #include <RaeptorCogs/Graphics/Text.hpp>

Which will give you access to the classes and functions needed to work with these graphics elements.

Initializing and terminating RæptorCogs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before using any RæptorCogs functionality, you must initialize the framework.
This is done by calling the :cpp:func:`RaeptorCogs::Initialize` function.
It is important to call this function at the beginning of your application, before creating any windows
or rendering graphics.

.. code-block:: cpp

   RaeptorCogs::Initialize();

At the end of your application, you should call :cpp:func:`RaeptorCogs::Destroy` to clean up resources
and properly terminate the framework.

.. code-block:: cpp

   RaeptorCogs::Destroy();

This ensures that all allocated resources are released and that the application exits cleanly.

Initializing the Renderer
~~~~~~~~~~~~~~~~~~~~~~~~~~
 
The Renderer is responsible for all rendering operations in RæptorCogs. Before you can render anything, you
need to initialize the Renderer with the desired graphics backend (e.g., OpenGL).

.. code-block:: cpp

   RaeptorCogs::Renderer().Initialize(RaeptorCogs::GraphicsBackend::GL);
   // Initialize OpenGL backend

This initializes the Renderer to use the OpenGL graphics backend. You can choose other backends if supported
by your system. This step is crucial as it sets up the necessary graphics context for rendering.

Creating a window
~~~~~~~~~~~~~~~~~

.. cpp:namespace:: RaeptorCogs::Singletons

To create a window where your graphics will be displayed, use the :cpp:func:`Platform::createWindow` method of the Renderer.
You can specify the width, height, and title of the window.

.. code-block:: cpp

    RaeptorCogs::Window* window = RaeptorCogs::Renderer().CreateWindow(
        800, 
        600, 
        "My First RæptorCogs Application"
        );

This creates a window with a width of 800 pixels, a height of 600 pixels, and the title "My First RæptorCogs Application".

Entering the main loop
~~~~~~~~~~~~~~~~~~~~~~~
After creating the window, you need to enter the main loop of your application. This loop will keep your application running,
handle events, and render graphics until the window is closed. To do this, use the :cpp:func:`RaeptorCogs::Renderer::StartLoop`
method. This method takes a function that will be called each frame to perform rendering and a pointer to the window you
created earlier.

.. code-block:: cpp

   RaeptorCogs::Renderer().StartLoop(
       [](RaeptorCogs::Window* win) {
           // Rendering code goes here
       },
       window
   );


Checking inputs
~~~~~~~~~~~~~~~~
Inside the main loop function, you can check for input events such as keyboard and mouse actions. RæptorCogs provides methods
to check the state of keys and mouse buttons. For example, to check if the Escape key is pressed, you can use the following code:

.. code-block:: cpp

   if (RaeptorCogs::Input().isKeyPressed(RaeptorCogs::Key::Escape)) {
       // Close the window if Escape is pressed
   }

Rendering with RæptorCogs
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Inside the main loop function, you can add your rendering code. This is where you will clear the screen, draw sprites, text, 
and other graphics elements. Here is a simple example of clearing the screen and rendering a sprite:

.. code-block:: cpp

   RaeptorCogs::Renderer().StartLoop(
       [](RaeptorCogs::Window* win) {
           // Create and render a sprite
           RaeptorCogs::Sprite2D sprite;
           sprite.setPosition({400.0f, 300.0f}); // Center of the window
           sprite.setSize({100.0f, 100.0f}); // Size of the sprite
           sprite.setColor({1.0f, 0.0f, 0.0f}); // Red color

           RaeptorCogs::Renderer().add(sprite); // Add sprite to the renderer
           RaeptorCogs::Renderer().render(*win); // Render all added elements
       },
       window
   );

Reading the timer
~~~~~~~~~~~~~~~~~~~~~
During each iteration of the main loop, you may want to read the time elapsed since the last frame. This is useful for
animations and time-based updates.
You can use the :cpp:func:`RaeptorCogs::Time::getDeltaTime` method to get the time in seconds since the last frame.

.. code-block:: cpp

   float deltaTime = RaeptorCogs::Time().getDeltaTime();

You can also get the total time since the application started using :cpp:func:`RaeptorCogs::Time::getTime`.

.. code-block:: cpp

   float totalTime = RaeptorCogs::Time().getTime();

Putting it together
~~~~~~~~~~~~~~~~~~~
Now that you have the basic components, you can put them together in a simple application. Below is a complete example
that initializes RæptorCogs, creates a window, and enters the main rendering loop.

.. literalinclude:: /_static/demo/src/main.cpp
    :language: cpp
    :linenos:

The program above can be found in the `repository <https://github.com/Estorc/RaeptorCogs>`_ as
`examples/demo/src/main.cpp <https://github.com/Estorc/RaeptorCogs/tree/main/examples/demo/src/main.cpp>`_.
This example demonstrates the basic structure of a RæptorCogs application, including initialization,
window creation, and the main rendering loop.