"* throws none
 * prints 1
 * prints 2
 * prints 3
 * prints 4
 * prints 5
 * prints 6
 *"
needle 2007;

"** Tests very basic array reading an writing. *"
"package test::array;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    [1,].[2,].[3,].[4,].[5,].[6,] -> { @item --
	out printLine: @item
    }
}
