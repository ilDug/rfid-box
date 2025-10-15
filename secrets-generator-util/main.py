import string
import secrets
from typing import Tuple


def generate_passphrase(length=64) -> str:
    """
    Generate a cryptographically secure random passphrase.

    Args:
        length (int): Length of the passphrase to generate (default: 64)

    Returns:
        str: Random passphrase containing uppercase, lowercase letters and digits
    """
    # Define character set: uppercase + lowercase + digits
    alphabet = string.ascii_letters + string.digits  # a-z, A-Z, 0-9

    # Use secrets module for cryptographically secure random generation
    passphrase = "".join(secrets.choice(alphabet) for _ in range(length))
    return passphrase


def generate_and_format_crypto_key() -> Tuple[str, str]:
    """
    Generate a random 6-byte cryptographic key for MIFARE Classic cards and format it as a C++ array initialization.

    Returns:
        str: Formatted C++ array string representing the crypto key (e.g., "{0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F}")
        str: Readable formatted string of the crypto key (e.g., "1A 2B 3C 4D 5E 6F")
    """
    key_bytes = [secrets.randbelow(256) for _ in range(6)]  # Generate 6 random bytes
    hex_values = [f"0x{byte:02X}" for byte in key_bytes]  # Format as hex strings
    readable_key_bytes = " ".join(
        f"{byte:02X}" for byte in key_bytes
    )  # Readable format
    formatted_cpp_key = "{" + ", ".join(hex_values) + "}"  # C++ array format

    return formatted_cpp_key, readable_key_bytes


def main():
    print("🔐 RFID Box Secrets Generator")
    print("=" * 50)

    # Generate new passphrase
    print("🎲 Generating new 64-character passphrase...")
    new_passphrase = generate_passphrase(64)
    print(f"✅ Generated passphrase: {new_passphrase}")

    # Generate new crypto key
    print("\n🔑 Generating new 6-byte crypto key...")
    key, bytes_str = generate_and_format_crypto_key()
    print(f"✅ Generated crypto key: {bytes_str}")

    print("\n" + "=" * 50)

    print("\n🔒 SECURITY NOTES:")
    print("- Keep these values secure and private")
    print("- Update data.h with the new crypto_key and passphrase")
    print("- Re-program all existing cards with the new key")
    print("- Backup these values in a secure location")

    print("📋 C++ CODE TO UPDATE:")
    print("=" * 50)

    print("For data.h - mainPassphrase:")
    print(f'       const String mainPassphrase = "{new_passphrase}";')

    print("For data.h - crypto_key:")
    print(f"       const byte crypto_key[MFRC522::MF_KEY_SIZE] = {key};")


if __name__ == "__main__":
    main()
