"* throws none
 * prints 83
 *"
needle 2007;

"** Tests creating and assigning to list literals. *"
"package test::list::assign"
public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: [92, 347, 192] {:[1]= 83; [1]}
}
