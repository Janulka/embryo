/*
 *  libembryo
 *  Copyright (C) 2010 by Jana Hlavacikova
 *
 *                  Version 3, December 2010
 
 *  Copyright (C) 2008 by Alexandre Devert
 *
 *           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                  Version 2, December 2004
 *
 *  Copyright (C) 2004 Sam Hocevar
 *  14 rue de Plaisance, 75014 Paris, France
 *  Everyone is permitted to copy and distribute verbatim or modified
 *  copies of this license document, and changing it is allowed as long
 *  as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#ifndef EMBRYO_PGM_H
#define EMBRYO_PGM_H

#include <iostream>
#include <Picture.h>



namespace embryo {
  // Create a Picture instance from a PGM file
  void loadPGM(std::istream& inStream, Picture & iPic);
//  Picture loadPGM(std::istream& inStream);

  // Save a Picture instance to a PGM file
  void savePGM(std::ostream& inStream, Picture& inPicture);
} // namespace embryo



#endif /* EMBRYO_PGM_H */
