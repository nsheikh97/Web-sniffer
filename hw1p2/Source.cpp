#include <unordered_set>
#include <string>
#include "pch.h"

using namespace std;

DWORD IP = inet_addr("128.194.135.72");
unordered_set<DWORD> seenIPs;
seenIPs.insert(IP);
...
//---------
unordered_set<string> seenHosts;
// populate with some initial elements
seenHosts.insert("www.google.com");
seenHosts.insert("www.tamu.edu");
string test = "www.cse.tamu.edu";
int prevSize = seenHosts.size();
seenHosts.insert(test);
if (seenHosts.size() > prevSize)
// unique host
else
// duplicate host