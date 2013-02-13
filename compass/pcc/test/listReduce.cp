"* throws none
 * prints 54
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: ([2, 40, 12] >< #+)
}
