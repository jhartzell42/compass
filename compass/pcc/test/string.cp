"* throws none
 * prints 42 things
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: [92, '42 things', 192][1]
}
