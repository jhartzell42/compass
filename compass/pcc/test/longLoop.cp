"* throws none
 * prints 5000050000
 *"
needle 2007;

"** Tests adding a lot of numbers on a range. *"
"package test::range::longLoop;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    out printLine: (1...100000 >< #+) 
}
