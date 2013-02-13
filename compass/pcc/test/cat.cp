"* throws none
 * reads 42 things
 * prints 42 things
 * reads
 * prints
 * reads a thing or two
 * prints a thing or two
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    {
	{
	    out printLine: in nextLine chomp
	} repeat;
    } onEndOfChannel: {
    }
}
