import string
import secrets

def generate_passphrase(length=64):
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
    passphrase = ''.join(secrets.choice(alphabet) for _ in range(length))
    
    return passphrase

def generate_crypto_key():
    """
    Generate a random 6-byte cryptographic key for MIFARE Classic cards.
    
    Returns:
        list: List of 6 random bytes (0x00-0xFF)
    """
    return [secrets.randbelow(256) for _ in range(6)]

def format_crypto_key(key_bytes):
    """
    Format crypto key as C++ array initialization.
    
    Args:
        key_bytes (list): List of 6 bytes
    
    Returns:
        str: Formatted C++ array string
    """
    hex_values = [f"0x{byte:02X}" for byte in key_bytes]
    return "{" + ", ".join(hex_values) + "}"

def generate_data_h_content(passphrase, crypto_key_bytes):
    """
    Generate the complete data.h file content with new secrets.
    
    Args:
        passphrase (str): The generated passphrase
        crypto_key_bytes (list): List of 6 random bytes for crypto key
    
    Returns:
        str: Complete data.h file content
    """
    crypto_key_formatted = format_crypto_key(crypto_key_bytes)
    default_key_formatted = format_crypto_key([0xFF] * 6)
    
    content = f"""#ifndef RFID_DATA_H
#define RFID_DATA_H

#include <MFRC522.h>

/************************************************/
/*** INSERIRE QUI I DATI DA SCRIVERE */
/************************************************/

// Firmware version for display and serial output
const String VERSION = "v1.0.0";

// main passphrase (max 135 characters) - GENERATED AUTOMATICALLY
const String mainPassphrase = "{passphrase}";

// ============================================================================
// CRYPTOGRAPHIC KEYS DECLARATIONS
// ============================================================================

/**
 * @brief MIFARE Classic Authentication Key (Constant Declaration)
 * @details External declaration of factory default 6-byte key for MIFARE Classic card authentication.
 *          This key is used for initial access to cards before any custom keys are set.
 *          Factory default key is widely known (FFFFFFFFFFFF) and should be changed for security.
 */
const byte default_key[MFRC522::MF_KEY_SIZE] = {default_key_formatted};

/**
 * @brief MIFARE Classic Authentication Key (Constant Declaration)
 * @details External declaration of custom 6-byte key for MIFARE Classic card authentication.
 *          Alternative to factory default key (FFFFFFFFFFFF).
 *          Used for enhanced security when reading/writing card data.
 *          This key must match the one programmed on the cards using the key-changer utility.
 *          GENERATED AUTOMATICALLY
 */
const byte crypto_key[MFRC522::MF_KEY_SIZE] = {crypto_key_formatted};

/************************************************/

#endif"""
    
    return content

def main():
    print("🔐 RFID Box Secrets Generator")
    print("=" * 50)
    
    # Generate new passphrase
    print("🎲 Generating new 64-character passphrase...")
    new_passphrase = generate_passphrase(64)
    print(f"✅ Generated passphrase: {new_passphrase}")
    
    # Generate new crypto key
    print("\n🔑 Generating new 6-byte crypto key...")
    new_crypto_key = generate_crypto_key()
    crypto_key_formatted = format_crypto_key(new_crypto_key)
    print(f"✅ Generated crypto key: {crypto_key_formatted}")
    
    # Show hexadecimal representation
    hex_representation = " ".join([f"{byte:02X}" for byte in new_crypto_key])
    print(f"   Hex representation: {hex_representation}")
    
    print("\n" + "=" * 50)
    print("📋 C++ CODE TO UPDATE:")
    print("=" * 50)
    
    print("\n1️⃣  For data.h - mainPassphrase:")
    print(f'const String mainPassphrase = "{new_passphrase}";')
    
    print("\n2️⃣  For data.h - crypto_key:")
    print(f"const byte crypto_key[MFRC522::MF_KEY_SIZE] = {crypto_key_formatted};")
    
    print("\n3️⃣  For key-changer.ino - customKey:")
    print(f"byte customKey[6] = {crypto_key_formatted};")
    
    print("\n" + "=" * 50)
    print("📄 COMPLETE data.h FILE:")
    print("=" * 50)
    
    # Generate complete data.h content
    complete_content = generate_data_h_content(new_passphrase, new_crypto_key)
    print(complete_content)
    
    # Ask user if they want to save to file
    print("\n" + "=" * 50)
    save_choice = input("💾 Do you want to save the complete data.h content to a file? (y/n): ").lower().strip()
    
    if save_choice in ['y', 'yes']:
        try:
            with open("generated_data.h", "w", encoding="utf-8") as f:
                f.write(complete_content)
            print("✅ File saved as 'generated_data.h'")
            print("   You can copy this content to your project's data.h file")
        except Exception as e:
            print(f"❌ Error saving file: {e}")
    
    print("\n🔒 SECURITY NOTES:")
    print("- Keep these values secure and private")
    print("- Update key-changer.ino with the new crypto_key")
    print("- Re-program all existing cards with the new key")
    print("- Backup these values in a secure location")


if __name__ == "__main__":
    main()
