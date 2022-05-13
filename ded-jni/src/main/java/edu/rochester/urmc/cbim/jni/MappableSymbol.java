
package edu.rochester.urmc.cbim.jni;


/**
 * A mappable symbol in the model. Mappable symbols are those variables or
 * macros that may appear in the data table.
 * 
 *
 */
public interface MappableSymbol {

	public static enum SortKey {
		OCCURRENCE_ASC, OCCURRENCE_DESC, NAME_ASC, NAME_DESC, DESCRIPTION_ASC, DESCRIPTION_DESC;

	}
	public String getName();
}
