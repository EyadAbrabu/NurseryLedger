
import subprocess
import tempfile
from pathlib import Path

# Find the compiled NurseryLedger executable.
ROOT = Path(__file__).resolve().parents[2]
APP = ROOT / "build" / "nurseryledger"


def run_nursery(commands, directory):
    """Start NurseryLedger and interact with its CLI."""
    result = subprocess.run(
        [str(APP)],
        input="\n".join(commands) + "\n",
        text=True,
        capture_output=True,
        cwd=directory,
        timeout=10
    )

    assert result.returncode == 0, result.stderr
    return result.stdout


def test_persistence():
    # Temporary directory prevents tests from touching real data.
    with tempfile.TemporaryDirectory() as directory:

        # First application session.
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "exit"
        ], directory)

        # Second application session.
        output = run_nursery([
            "children",
            "bills 101",
            "payments 101",
            "credit 101",
            "exit"
        ], directory)

        assert "Ahmed" in output
        assert "PAYMENT HISTORY" in output
        assert "BILLING HISTORY" in output

        # Check the actual CSV values, not only the CLI messages.
        data = Path(directory)
        assert "101,Ahmed,2026,11,15,15000" in (
            data / "children.csv"
        ).read_text()

        assert "101,2026,11,20,20000,cash,2026,11" in (
            data / "payments.csv"
        ).read_text()

        assert len((data / "obligations.csv").read_text().splitlines()) == 2

        print("PASS: Children, bills, and payments survive restart")



def test_duplicate_child_id():
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "register 101 Omar 2026-11-15 200.00",
            "exit"
        ], directory)

        csv_file = Path(directory) / "children.csv"
        rows = csv_file.read_text().splitlines()

        # Header + exactly one registered child.
        assert len(rows) == 2, (
            f"Expected 2 CSV lines, found {len(rows)}: {rows}"
        )

        assert rows[1] == "101,Ahmed,2026,11,15,15000", (
            f"Unexpected child record: {rows[1]}"
        )

        print("PASS: Duplicate child ID rejected")




def test_duplicate_monthly_bill():
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "bill 101 2026 11",
            "exit"
        ], directory)

        rows = (
            Path(directory) / "obligations.csv"
        ).read_text().splitlines()

        # One header + exactly one monthly bill.
        assert len(rows) == 2, (
            f"Duplicate billing detected: {rows}"
        )

        # Verify the bill belongs to the correct child and period.
        assert rows[1] == "101,2026,11,2026,11,15,15000", (
            f"Incorrect billing record: {rows[1]}"
        )

        print("PASS: Duplicate monthly billing prevented")



def test_payment_without_bill():
    """A parent cannot pay against a bill that doesn't exist."""
    with tempfile.TemporaryDirectory() as directory:
        output = run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "pay 101 2026-11-20 100.00 cash 2026 11",
            "exit"
        ], directory)

        rows = (
            Path(directory) / "payments.csv"
        ).read_text().splitlines()

        assert len(rows) == 1, (
            f"Payment was accepted without a bill: {rows}"
        )

        print("PASS: Payment without an existing bill rejected")


def test_partial_payment():
    """Paying part of a bill should leave the correct amount due."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 60.00 cash 2026 11",
            "exit"
        ], directory)

        output = run_nursery([
            "balance 101 2026 11",
            "exit"
        ], directory)

        assert "90.00" in output, (
            f"Expected 90.00 EUR outstanding, got:\n{output}"
        )

        print("PASS: Partial payment leaves 90.00 EUR outstanding")


def test_overpayment_credit():
    """Extra money should become credit, not disappear into the void."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "exit"
        ], directory)

        output = run_nursery([
            "credit 101",
            "exit"
        ], directory)

        assert "50.00" in output, (
            f"Expected 50.00 EUR credit, got:\n{output}"
        )

        print("PASS: Overpayment creates 50.00 EUR credit")





def test_credit_carries_forward():
    """November's extra money should help pay December's bill."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "bill 101 2026 12",
            "exit"
        ], directory)

        output = run_nursery([
            "balance 101 2026 12",
            "credit 101",
            "exit"
        ], directory)

        # November's extra 50 EUR should reduce December's bill.
        assert "100.00" in output, (
            f"Expected December balance of 100.00 EUR:\n{output}"
        )

        print("PASS: Credit carries forward to next month's bill")


def test_refund_exceeds_credit():
    """We can't refund money that isn't available anymore."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "refund 101 2026-11-21 70.00 cash 2026 11",
            "exit"
        ], directory)

        refund_rows = (
            Path(directory) / "refunds.csv"
        ).read_text().splitlines()

        # Only 50 EUR of credit exists, so 70 EUR must be rejected.
        assert len(refund_rows) == 1, (
            f"Excessive refund was accepted: {refund_rows}"
        )

        print("PASS: Refund exceeding available credit rejected")


def test_valid_refund_reduces_credit():
    """A valid refund should reduce credit by exactly that amount."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "refund 101 2026-11-21 30.00 cash 2026 11",
            "exit"
        ], directory)

        output = run_nursery([
            "credit 101",
            "exit"
        ], directory)

        # 200 paid - 150 billed - 30 refunded = 20 EUR.
        assert "20.00" in output, (
            f"Expected 20.00 EUR remaining credit:\n{output}"
        )

        refund_rows = (
            Path(directory) / "refunds.csv"
        ).read_text().splitlines()

        assert len(refund_rows) == 2, (
            f"Expected exactly one refund: {refund_rows}"
        )

        print("PASS: Valid refund reduces available credit")





def test_refund_consumed_credit():
    """Once credit pays a future bill, we can't refund it again."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "bill 101 2026 12",
            "refund 101 2026-12-20 50.00 cash 2026 11",
            "exit"
        ], directory)

        rows = (Path(directory) / "refunds.csv").read_text().splitlines()

        assert len(rows) == 1, (
            f"Already-consumed credit was refunded: {rows}"
        )

        print("PASS: Already-consumed credit cannot be refunded")


def test_all_records_survive_restart():
    """Children, bills, payments and refunds must all survive a restart."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "refund 101 2026-11-21 30.00 cash 2026 11",
            "exit"
        ], directory)

        run_nursery(["exit"], directory)

        data = Path(directory)

        expected_rows = {
            "children.csv": "101,Ahmed,2026,11,15,15000",
            "obligations.csv": "101,2026,11,2026,11,15,15000",
            "payments.csv": "101,2026,11,20,20000,cash,2026,11",
            "refunds.csv": "101,2026,11,21,3000,cash,2026,11",
        }

        for filename, expected in expected_rows.items():
            rows = (data / filename).read_text().splitlines()
            assert rows.count(expected) == 1, (
                f"{filename} lost or duplicated a record: {rows}"
            )

        print("PASS: All four CSV record types survive restart")


def test_repeated_restarts():
    """Restarting the application shouldn't magically multiply money."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "refund 101 2026-11-21 30.00 cash 2026 11",
            "exit"
        ], directory)

        data = Path(directory)
        files = [
            "children.csv",
            "obligations.csv",
            "payments.csv",
            "refunds.csv",
        ]

        original = {
            name: (data / name).read_text()
            for name in files
        }

        for _ in range(3):
            run_nursery(["exit"], directory)

        for name in files:
            current = (data / name).read_text()
            assert current == original[name], (
                f"{name} changed after repeated restarts"
            )

        print("PASS: Repeated restarts preserve records exactly")



def test_corrupted_csv_preserves_data():
    """Bad CSV data should never be silently overwritten."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "exit"
        ], directory)

        csv_file = Path(directory) / "children.csv"

        # Simulate a corrupted record.
        corrupted = (
            "child_id,name,year,month,day,monthly_fee_cents\n"
            "101,Ahmed,NOT_A_YEAR,11,15,15000\n"
        )
        csv_file.write_text(corrupted)

        # Loading should fail instead of silently starting fresh.
        result = subprocess.run(
            [str(APP)],
            input="exit\n",
            text=True,
            capture_output=True,
            cwd=directory,
            timeout=10
        )

        assert result.returncode != 0, (
            "Application accepted corrupted CSV data"
        )

        assert csv_file.read_text() == corrupted, (
            "Application overwrote corrupted CSV data"
        )

        print("PASS: Corrupted CSV rejected without overwriting data")


def test_invalid_money_input():
    """Invalid money shouldn't become a real financial transaction."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 -50.00 cash 2026 11",
            "pay 101 2026-11-20 12.345 cash 2026 11",
            "pay 101 2026-11-20 abc cash 2026 11",
            "exit"
        ], directory)

        rows = (
            Path(directory) / "payments.csv"
        ).read_text().splitlines()

        assert len(rows) == 1, (
            f"Invalid payment was saved: {rows}"
        )

        print("PASS: Invalid monetary inputs rejected")


def test_failed_registration_preserves_data():
    """One bad registration shouldn't ruin an existing nursery."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "exit"
        ], directory)

        run_nursery([
            "register 102 Omar 2026-11-10 200.00",
            "exit"
        ], directory)

        rows = (
            Path(directory) / "children.csv"
        ).read_text().splitlines()

        assert len(rows) == 2, (
            f"Unexpected child records: {rows}"
        )

        assert "101,Ahmed,2026,11,15,15000" in rows
        assert not any("Omar" in row for row in rows)

        print("PASS: Failed registration preserves existing data")



if __name__ == "__main__":
    test_persistence()
    test_duplicate_child_id()
    test_duplicate_monthly_bill()
    test_payment_without_bill()
    test_partial_payment()
    test_overpayment_credit()
    test_credit_carries_forward()
    test_refund_exceeds_credit()
    test_valid_refund_reduces_credit()
    test_refund_consumed_credit()
    test_all_records_survive_restart()
    test_repeated_restarts()
    test_corrupted_csv_preserves_data()
    test_invalid_money_input()
    test_failed_registration_preserves_data()
