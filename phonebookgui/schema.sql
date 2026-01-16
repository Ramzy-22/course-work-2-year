-- =====================================================
-- PHONEBOOK DATABASE SCHEMA
-- =====================================================

-- Drop existing tables (for clean setup)
DROP TABLE IF EXISTS contacts CASCADE;
DROP TABLE IF EXISTS phones CASCADE;
DROP SEQUENCE IF EXISTS contacts_id_seq CASCADE;

-- Create sequence for auto-incrementing IDs
CREATE SEQUENCE contacts_id_seq START 1;

-- =====================================================
-- CONTACTS TABLE
-- =====================================================
CREATE TABLE contacts (
    id INTEGER PRIMARY KEY DEFAULT nextval('contacts_id_seq'),
    first_name VARCHAR(100) NOT NULL,
    middle_name VARCHAR(100),
    last_name VARCHAR(100) NOT NULL,
    email VARCHAR(255) NOT NULL UNIQUE,
    address TEXT,
    birthday DATE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    -- Constraints
    CONSTRAINT chk_first_name CHECK (first_name ~ '^[A-Za-z][A-Za-z0-9 -]*[^-]$'),
    CONSTRAINT chk_last_name CHECK (last_name ~ '^[A-Za-z][A-Za-z0-9 -]*[^-]$'),
    CONSTRAINT chk_email CHECK (email ~ '^[A-Za-z0-9]+@[A-Za-z0-9]+$')
);

-- =====================================================
-- PHONES TABLE (One-to-Many relationship)
-- =====================================================
CREATE TABLE phones (
    id SERIAL PRIMARY KEY,
    contact_id INTEGER NOT NULL REFERENCES contacts(id) ON DELETE CASCADE,
    phone_type VARCHAR(20) NOT NULL, -- 'work', 'home', 'office'
    phone_number VARCHAR(30) NOT NULL,
    
    -- Constraints
    CONSTRAINT chk_phone_type CHECK (phone_type IN ('work', 'home', 'office')),
    CONSTRAINT chk_phone_format CHECK (
        phone_number ~ '^(\+7|8)(\d{10}|\(\d{3}\)\d{7}|\(\d{3}\)\d{3}-\d{2}-\d{2})$'
    ),
    CONSTRAINT unq_contact_phone UNIQUE (contact_id, phone_type)
);

-- =====================================================
-- INDEXES for Performance
-- =====================================================
CREATE INDEX idx_contacts_first_name ON contacts(first_name);
CREATE INDEX idx_contacts_last_name ON contacts(last_name);
CREATE INDEX idx_contacts_email ON contacts(email);
CREATE INDEX idx_phones_contact_id ON phones(contact_id);
CREATE INDEX idx_phones_number ON phones(phone_number);

