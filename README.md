<p align="center">
  <img src="https://github.com/skapaflow/MyDDB/blob/master/icon/MyDDB.png" />
</p>

<h1 align="center">My Domestic Database</h1>

<b>MyDDB</b> is a simple database management system write in C,
created to be simple and small, it's very useful in projects with non-sensitive data,
because the data is stored in a flat-file with only rows and columns.
using a relational database model, and simple commands CRUD-based (Create Read Update Delete).

MyDDB code is portable for Linux and Windows.

---

## Building

MyDDB is self-contained in two files:
[myddb.h](https://www.google.com/skapaflow/MyDDB)
and
[myddb.c](https://www.google.com/skapaflow/MyDDB)
To use MyDDB, simply add those two files to your project.

## Example Code
* [myddb_ex1.c](https://www.google.com/skapaflow/MyDDB)

## Sintax
It's a very simple sintax, has just two elements: (a "command" followed by an "argument") some commands has no argument.
#### Some examples:
```python
PATH "../base"  #set a directory path
FROM "data"     #set a database
PRINT 1         #print in long format
EXCLUDE "data"  #explude a database
CREATE "data"   #create a database (file: data.ddb)
SHOW            #show database files
VER             #show myddb version
HELP            #show help commands
```
## Usage

> myddb main function it's like printf in c.
```c
myddb_open();
int value = 1;
myddb("FROM helloworld PRINT %d", value);
myddb_close();
```

#### output
```
+--+-----+-----+
|id| name| time|
+--+-----+-----+
| 0|Alice| AD 1|
| 1|  Bob| 1 BC|
+--+-----+-----+
```
## Data Structure

* The data in MyDDB file, are divided in two non-printable ASCII characters.

DEC|HEX|CHARACTER|
--- | --- | ---
3|3|END OF TEXT (ETX)|
29|1D|GROUP SEPARATOR (GS) LEFT ARROW|

* Inside helloworld.ddb file, you'll see this.

<dl>
  <dd>
      id <b>GS</b> name <b>GS</b> time <b>ETX</b> 0 <b>GS</b> Alice <b>GS</b> AD 1 <b>ETX</b> 1 <b>GS</b> Bob <b>GS</b> 1 BC <b>ETX</b>
  </dd>
</dl>

## Tests

* [myddb_test.c](https://github.com/skapaflow/MyDDB/blob/master/myddb_test.c)

## License

The code of MyDDB is licensed under [MIT License](https://github.com/skapaflow/MyDDB/blob/master/LICENSE).
