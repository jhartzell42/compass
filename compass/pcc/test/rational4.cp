"* throws none
 * prints 81/256
 * prints 7
 * prints 5/6
 * prints 2
 * prints 3/8
 * prints 3/4
 * prints 18
 * prints 17/6
 *"
needle 2007;

"** Tests creating and reading from list literals. *"
"package test::list"
public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    [81/256, 3/5 + 32/5, 10/12, 3*(2/3), 1/2*(3/4),
		1/2/(2/3), 15/5*6, 2+10/12] -> { @num --
	out printLine: "0-(0-(0+1*("@num"-0)))"
    }
}
