"* throws none
 * prints 81
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"
public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: ($Test==$Test?{81}//{'ERROR!'})
}
