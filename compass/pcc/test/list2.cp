"* throws none
 * prints 4
 * prints 5
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"
public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    ([1,2,3,4,5][3~4~1]) -> { @toPrint --
	out printLine: @toPrint
    }
}
