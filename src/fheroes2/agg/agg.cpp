/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2019 - 2026                                             *
 *                                                                         *
 *   Free Heroes2 Engine: http://sourceforge.net/projects/fheroes2         *
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <list>
#include <stdexcept>
#include <utility>

#include "agg.h"
#include "agg_file.h"
#include "dir.h"
#include "settings.h"
#include "tools.h"

namespace
{
    fheroes2::AGGFile heroes2_agg;
    fheroes2::AGGFile heroes2x_agg;
}

std::vector<uint8_t> AGG::getDataFromAggFile( const std::string & key, const bool ignoreExpansion )
{
    if ( !ignoreExpansion && heroes2x_agg.isGood() ) {
        std::vector<uint8_t> buf = heroes2x_agg.read( key );
        if ( !buf.empty() ) {
            return buf;
        }
    }

    return heroes2_agg.read( key );
}

AGG::AGGInitializer::AGGInitializer()
{
    if ( init() ) {
        return;
    }

    throw std::logic_error( "No AGG data files found." );
}

bool AGG::AGGInitializer::init()
{
    const ListFiles aggFileNames = Settings::FindFiles( "data", ".agg", false );

    if ( aggFileNames.empty() ) {
        return false;
    }

    const std::string heroes2AggFileName = "heroes2.agg";
    std::string heroes2AggFilePath;
    std::string aggLowerCaseFilePath;

    for ( const std::string & path : aggFileNames ) {
        const std::string tempPath = StringLower( path );

        if ( tempPath.size() >= heroes2AggFileName.size()
             && tempPath.compare( tempPath.size() - heroes2AggFileName.size(), heroes2AggFileName.size(), heroes2AggFileName ) == 0 ) {
            heroes2AggFilePath = path;
            aggLowerCaseFilePath = tempPath;
            break;
        }
    }

    if ( heroes2AggFilePath.empty() ) {
        return false;
    }

    if ( !heroes2_agg.open( heroes2AggFilePath ) ) {
        return false;
    }

    _originalAGGFilePath = std::move( heroes2AggFilePath );

    std::string heroes2XAggFilePath;
    fheroes2::replaceStringEnding( aggLowerCaseFilePath, ".agg", "x.agg" );

    for ( const std::string & path : aggFileNames ) {
        const std::string tempPath = StringLower( path );

        if ( tempPath == aggLowerCaseFilePath ) {
            heroes2XAggFilePath = path;
            break;
        }
    }

    if ( !heroes2XAggFilePath.empty() && heroes2x_agg.open( heroes2XAggFilePath ) ) {
        _expansionAGGFilePath = std::move( heroes2XAggFilePath );
    }

    Settings::Get().EnablePriceOfLoyaltySupport( heroes2x_agg.isGood() );

    return true;
}

bool AGG::isPoLResourceFilePresent()
{
    return heroes2x_agg.isGood();
}
