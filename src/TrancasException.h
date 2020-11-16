/* Copyright (C) 2013 Miguel Rodríguez Pérez <miguel@det.uvigo.es>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Authors: Miguel Rodríguez Pérez <miguel@det.uvigo.es>
 *
 */

#ifndef TRANCASEXCEPTION_H
#define	TRANCASEXCEPTION_H

#include <exception>
#include <string>

class TrancasException : public std::exception {
public:
    TrancasException(const std::string& reason) noexcept : std::exception(), reason(reason) {}
    
    virtual const char *what() const noexcept;
private:
    std::string reason;
};

#endif	/* TRANCASEXCEPTION_H */

