*****	PURPOSE

This contains the "simplified network programming API" and client server programs
for demonstrating the "echo" service.



To Run/Build:


1.	Compile all source code files in your favourite C++ IDE ...
	Add the directory "./" to your Project/General properties "Additional Include Directories"
	OR Copy the cnaiapi.h and cnaiapi_win32.h files into the default include
	directory for your IDE

		e.g.	For visual studio installed on C drive using the "Release" configuration (not the DEBUG config.)

				FOR THE C++ Property, select the "GENERAL" tab, ADD the "./" folder to the 
				
				Additional Include Directories: ./

				OR  COPY THE INCLUDE FILES TO THE FOLDER

				c:\microsoft visual studio\vc98\include 

	Compile the simple network programming API first (folder <root.api>)
	You may use the Solution view, right click each C++ source code file and select compile.
	If you use the "BUILD" command, the project will attempt to "link" and will fail.
	Optionally create an object module library (see next step)
	Compile the client and server programs (folder <root.apps>)

2.	Create a library for the object code (*.obj files need to be in your default directory):

	>lib /out:cnaiapi_32.lib *.obj

	Creates a library called CNAIAPI_32.lib containing all object code
	modules created by the compile step.
	
3.	Add the "CNAIAPI_32.lib" to your IDE link step along with the windows
	socket library version 2
		"CNAIAPI_32.lib;Ws2_32.lib;%(AdditionalDependencies)" (under linker/input "Additional Dependencies").


	... HAVE FUN ...


*****	CONTENTS
<root>
<DIR>		api			Simplified Network Programming API directory
<DIR>		apps			Echo client and server application directory
readme.txt				This readme file
cnaiapi_32.lib				A prebuilt library containing the object code
					for the simplified network programming API

<root.api>
send_eof.cpp				used by both client and server after they have finished sending data
await_contact.cpp			used by a server to wait for contact from a client
cname_to_comp.cpp			used to translate a computer name to an equivalent internal binary value
appname_to_appnum.cpp	used to translate a program name to an equivalent internal binary value
make_contact.cpp			used by a client to contact a server
cnaiapi_init.cpp			Initialize the cnaiapi library.
								Initialize Winsock (if Win32) and create the synchronization
								mutexes
cnaiapi_win32.h				Its a windows 32 bit version
cnaiapi.h				Simplified Network Programming API header file

<root.apps>
echoclient.cpp				Client source code for the "echo" service
readln.cpp				Read from stdin until newline or EOF, or buffer is full
echoserver.cpp				Server source code for the "echo" service

***


*****	MODIFICATIONS TO MAKE
IF YOU WISH TO REGISTER A DLL:
To register a DLL: regsvr32 <dll name>.dll
To unregister a DLL: regsvr32 -u <dll name>.dll

AS AN ALTERNATIVE add the appropriate folder/directory to your system level "PATH" environment variable