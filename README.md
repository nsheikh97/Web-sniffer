# Web-sniffer
given a single URL or text file of URLS, crawls page and checks for links on a given page

## How it works
1. Looks up IP with DNS lookup
2. Connect to socket on server
3. Send HEAD Request for /robots.txt
4. Load data
5. Verify Header  ~  
*If host does not contain robots.txt, continue...*
6. Repeat 1-5 with GET request (Since HTTP 1.0, reconnection to socket is mandatory)  ~  
*If host returns with 2xx status code, continue...*
7. Parse HTML and return # of links

## How to run:

### Windows:

```sh
hw1p2.exe <URL>
```
*or*

```sh
hw1p2.exe <# of threads> <text file>
```
### Example input:

**test.txt**
```sh
http://www.google.com
http://irl.cs.tamu.edu

```
### Example Output:

```sh
Opened test.txt with size 47
URL: http://www.google.com
        Parsing URL... host www.google.com, port 80
        Checking host uniqueness...passed
        Doing DNS...done in 4 ms Found 216.58.193.132
        Checking IP uniqueness...passed
        Connecting on robots...done in 16 ms
        Loading...done in 24 ms with 326 bytes
        Verifying Header...status code 200
URL: http://irl.cs.tamu.edu
        Parsing URL... host irl.cs.tamu.edu, port 80
        Checking host uniqueness...passed
        Doing DNS...done in 0 ms Found 128.194.135.72
        Checking IP uniqueness...passed
        Connecting on robots...done in 9 ms
        Loading...done in 18 ms with 155 bytes
        Verifying Header...status code 404
       *Connecting on page...done in 10 ms
        Loading...done in 33 ms with 6885 bytes
        Verifying Header...status code 200
       +Parsing page...done in 0 ms with 11 links
```

## Version:

**1.0** // Currently only support 1 thread

## Experience
+ Using WinSock2
+ Using recv with dynamic buffer
+ Performing DNS lookup
+ Sending GET and HEAD HTTP requests
