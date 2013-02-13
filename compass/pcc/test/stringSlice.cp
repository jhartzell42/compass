"* throws none
 * prints 123456
 * prints 357
 * prints 37
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: '0123456789'[1~6~1];
    out printLine: '0123456789'[3~7~2];
    out printLine: '0123456789'[3~7~2][0~2~2];
}
