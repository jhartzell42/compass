"* throws none
 * reads Foo
 * prints Foo
 * reads Bar
 * prints Bar
 * reads End
 * prints End
 * reads a whole line of stuff
 * prints a whole line of stuff
 * reads
 * prints
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @lineIn := in lineReader;
    {
	{
	    out printLine: @lineIn next chomp;
	} repeat;
    } onEndOfChannel: {
    }
}
