"* throws none
 * prints 81/256
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"
public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: [92, 81/256, 192][1]
}
