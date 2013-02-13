"* throws none
 * prints We have 42 things in here.
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: (['We have ', '42 things', ' in here.'] >< #.)
}
