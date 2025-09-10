package com.chtholly.chthl.evaluator;

import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * A data class to represent a numeric value and its associated unit,
 * e.g., 100 and "px".
 */
public class UnitValue {
    public final double value;
    public final String unit; // e.g., "px", "%", "em", or "" for unitless numbers

    public UnitValue(double value, String unit) {
        this.value = value;
        this.unit = Objects.requireNonNull(unit);
    }

    public static UnitValue fromString(String s) {
        if (s == null || s.trim().isEmpty()) return new UnitValue(0, "");

        Pattern pattern = Pattern.compile("^([\\-+]?[0-9]*\\.?[0-9]+)(.*)$");
        Matcher matcher = pattern.matcher(s.trim());
        if (matcher.matches()) {
            return new UnitValue(Double.parseDouble(matcher.group(1)), matcher.group(2).trim());
        }
        // Fallback for non-numeric values that might be treated as zero
        return new UnitValue(0, "");
    }

    @Override
    public String toString() {
        // Handle integers cleanly without a ".0"
        if (unit.isEmpty()) {
             if (value == (long) value) return String.format("%d", (long)value);
             return String.format("%s", value);
        }
        if (value == (long) value) {
            return String.format("%d%s", (long)value, unit);
        } else {
            return String.format("%s%s", value, unit);
        }
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        UnitValue unitValue = (UnitValue) o;
        return Double.compare(unitValue.value, value) == 0 && unit.equals(unitValue.unit);
    }

    @Override
    public int hashCode() {
        return Objects.hash(value, unit);
    }
}
