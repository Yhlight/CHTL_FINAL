package com.chtholly.chthl.evaluator;

import java.util.Objects;

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

    @Override
    public String toString() {
        // Handle integers cleanly without a ".0"
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
