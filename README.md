This hack is to provide functions useful with IPv4 IP Address

Exposed API
-----------

*inet.aton* : This will convert given dotted IP Address to integer form.
This will work with argument as string or a key with string value.

*inet.ntoa* : This will convert given integer IP Address to dotted form.
This will work with argument as string or a key with string value.

*inet.exists*: Takes two arguments of IPv4 address in dotted or integer form. Will compare them
and return YES for a match and NO for no match. The second argument can be a dotted/integer address or
a key with string value.

Building the module
-------------------
% gcc -fPIC -std=gnu99 -c -o ntoa_aton.o inet_aton_ntoa.c 
% ld -o inet_aton_ntoa.so ntoa_aton.o -shared -Bsymbolic -lc

Loading the module in Redis
---------------------------
% ./redis-server --loadmodule /path/to/generated/library/inet_aton_ntoa.so

Sample Calls
------------

127.0.0.1:6379> INET.ATON 10.10.132.111
(integer) 168461423

127.0.0.1:6379> INET.NTOA 168461423
"10.10.132.111"

127.0.0.1:6379> INET.ATON 258.10.1.1
(error) Invalid IPv4 format

127.0.0.1:6379> INET.NTOA 4294967296
(error) Invalid IPv4 Address Range

127.0.0.1:6379> INET.NTOA 494967296
"29.128.154.0"

127.0.0.1:6379> SET k1 10.10.132.111
OK
127.0.0.1:6379> SET k2 168461423
OK
127.0.0.1:6379> SET k3 258.10.1.1
OK
127.0.0.1:6379> SET k4 4294967296
OK
127.0.0.1:6379> SET k5 494967296
OK

127.0.0.1:6379> INET.ATON k1
(integer) 168461423

127.0.0.1:6379> INET.NTOA k2
"10.10.132.111"
127.0.0.1:6379> INET.ATON k3
(error) Invalid IPv4 format
127.0.0.1:6379> INET.NTOA k4
(error) Invalid IPv4 Address Range
127.0.0.1:6379> INET.NTOA k5
"29.128.154.0"
127.0.0.1:6379> INET.EXISTS k1 k1
TRUE
127.0.0.1:6379> INET.EXISTS k1 k2
TRUE

127.0.0.1:6379> INET.EXISTS k2 k1
TRUE

127.0.0.1:6379> INET.EXISTS k3 k2
(error) Invalid Needle IPv4 IP Address

127.0.0.1:6379> INET.EXISTS k3 k1
(error) Invalid IPv4 IP Address parameters to function     
                                                               
127.0.0.1:6379> INET.EXISTS k4 k5
(error) Invalid IPv4 address                          
                                                                    
127.0.0.1:6379> INET.EXISTS k5 494967296
TRUE

127.0.0.1:6379> SET k6 168461423
OK
127.0.0.1:6379> SET k7 10.10.199.111
OK

127.0.0.1:6379> INET.EXISTS 10.10.132.111 k6
TRUE

127.0.0.1:6379> INET.EXISTS k6 10.10.1.1
FALSE

127.0.0.1:6379> INET.EXISTS k6 10.10.132.111
TRUE

127.0.0.1:6379> INET.EXISTS 10.10.132.111 10.10.132.111
TRUE

127.0.0.1:6379> INET.EXISTS 10.10.132.111 k7
FALSE

127.0.0.1:6379> INET.EXISTS k7 10.10.199.111
TRUE                                                                                                                                                     

TODO
----

In the future, a data type to hold the IP Address in both notations (or just one, as provided during set operation) can be introduced. This will provide readily available ip addresses in both forms and hence help perform faster operations using them. Say, we want to match specific IP Address' and have set them in Redis. The address that comes in (which may be in either of the two notations),  can be matched in dotted or integer form in a single API call at the redis layer itself. This way we will have faster processing, for time crucial applications (although at the cost of higher memory footprint).

* Functions _exists_ will also take a set as 2nd argument. It will indicate if the 1st argument is present in the set.
* ntoa and aton API will return an array incase of key of type list
* Data structure for IPv4 IP address
* SET IP address will provide options like store both forms or just one
* GET key will also provide options for notation of retrieved value
* Function _exists_ will match given data against both notations.
