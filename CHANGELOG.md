# Changelog

## Limitations
Only linux implementation.
Server is not terminating connection when tcp client is just connected without any action for a while.
(because of strictly set textual format server is always waiting for newline in message)



## Changes

### Commit: Add comments, refactorize
Date:   Apr 17 2023
    
    - added comments, headers, refactorize

### Commit: Fix message loading, add more udp validation
Date:   Apr 17 2023
    
    - removed old tcp message recieving with new that supports more queries in 1 message and 1 query in lot of messages
    - added udp client messages validation
    - fixed buffer first bytes setting
    
### Commit: Add sigint handler for tcp module
Date:   Apr 17 2023
    
    - added mutexes
    - added option to remove client from vector in thread when exiting
    - added loop that closes every active socket after sigint
    - added vector for every thread and then waiting for every thread to exit

### Commit: Add additional fixes to buffer setting
Date:   Apr 17 2023

    - fixed buffer indexing

### Commit: Fix error throwing 
Date:   Apr 17 2023
    
    - changed prints to streams because of threads
    - changed throwing numbers

    
### Commit: Add sigint handler, tweak udp buffer handling
Date:   Apr 17 2023

    - added siging handler that sets bool flag 
    - fixed some errors in udp

### Commit: Add tcp implementation 
Date:   Apr 16 2023

    - added class for tcp connection
    - added method listen that is waiting for client connection and then creating threads for each client
    - added function for handling each client thread

### Commit: Add udp connection implementation
Date:   Apr 15 2023

    - added class for udp connection
    - added method listen that is waiting for client message

### Commit: Fix makefile dependencies 
Date:   Apr 15 2023

    - changed .hpp into .h in makefile

### Commit: Add command line arguments loading 
Date:   Apr 15 2023

    - added simple commad line argumets parsing and validating
    - added printing function for help

### Commit: Add enumeration function  
Date:   Apr 15 2023

    - added function into parser that enumerates query

### Commit: Add syntax parser implementation, add makefile  
Date:   Apr 15 2023

    - added tokeniser class for loading tokens from input 
    - added parser class for checking query syntax

## Bibliography
[C++ Documentation](https://learn.microsoft.com/en-us/cpp/cpp/?view=msvc-170)  
[STUBS](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs)
[Lectures](https://moodle.vut.cz/mod/folder/view.php?id=289124)