/*******************************************************************//**
 *  \file noopcart.h
 *  \author btrim
 *  \brief This program allows to read and write to various game cartridges.
 *         The UMD base class handles all generic cartridge operations, console
 *         specific operations are handled in derived classes.
 *
 * \copyright This file is part of Universal Mega Dumper.
 *
 *   Universal Mega Dumper is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Universal Mega Dumper is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Universal Mega Dumper.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CARTRIDGES_NOOPCART_H_
#define CARTRIDGES_NOOPCART_H_

#include "Cartridge.h"

/*******************************************************************//**
 * \class NoCart
 * \brief
 **********************************************************************/
class NoCart : public Cartridge
{
public:

	/*******************************************************************//**
	 * \brief Constructor
	 **********************************************************************/
	NoCart(){}

	/*******************************************************************//**
	 * \brief setup the UMD's hardware for the current cartridge
	 * \return void
	 **********************************************************************/
	virtual void init(){}

};

#endif /* CARTRIDGES_NOOPCART_H_ */
