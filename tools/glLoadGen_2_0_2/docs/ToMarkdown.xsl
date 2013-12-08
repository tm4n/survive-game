<?xml version="1.0" encoding="UTF-8"?>
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xd="http://www.oxygenxml.com/ns/doc/xsl"
    xmlns:db="http://docbook.org/ns/docbook"
    xmlns:pg="https://bitbucket.org/alfonse/glloadgen"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    xmlns:fn="http://www.w3.org/2005/xpath-functions"
    exclude-result-prefixes="xs xd"
    version="2.0">
    
    <xsl:strip-space elements="*"/>
    <xsl:output method="text" encoding="UTF-8"/>
    <xsl:output name="text" method="text" encoding="UTF-8"/>
    
    <xsl:param name="filePrefix"/>
    <xsl:param name="basename"/>
    
    
    <xsl:template match="/">
        <xsl:apply-templates select="/db:article/*" mode="root"/>
    </xsl:template>
    
    <!-- Basic structural controls. -->
    <!--
    <xsl:template match="db:section" mode="root">
        <xsl:choose>
            <xsl:when test="name(preceding-sibling::*[1]) != 'title' and
                name(preceding-sibling::*[1]) != 'section'">
                <xsl:apply-templates select="." mode="file"/>
            </xsl:when>
            <xsl:when test="ancestor::db:article/processing-instruction(together)">
                <xsl:apply-templates select="." mode="file"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="baseName" select="if(empty(processing-instruction(trope))) then
                    (string(db:title)) else (substring-before(
                    substring-after(string(processing-instruction(trope)), 'url=&quot;'), '&quot;'))"/>
                <xsl:variable name="sectionNum" select="index-of(../db:section, .)"/>
                <xsl:variable name="prefixedName" select="
                    if(empty($filePrefix)) then
                        (concat($sectionNum, ' - ', $baseName))
                    else
                        (concat($filePrefix, ' - ', $sectionNum, ' - ', $baseName))"/>
                <xsl:variable name="filename" select="concat($prefixedName, '.txt')"/>
                
                <xsl:result-document format="text" href="{$filename}">
                    <xsl:apply-templates select="*" mode="file"/>
                </xsl:result-document>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    -->

    <xsl:template match="db:title" mode="root"/>

    <xsl:template match="db:*" mode="root">
        <xsl:apply-templates select="." mode="file"/>
    </xsl:template>
    
    
    <!-- Process the actual text data. -->
    <xsl:template match="db:para" mode="file">
        <xsl:variable name="listCount" select="count(ancestor::db:listitem)"/>
        <xsl:if test="ancestor::db:blockquote or ancestor::db:epigraph">
            <xsl:text>></xsl:text>
        </xsl:if>
        <xsl:if test="ancestor::db:itemizedlist and not(preceding-sibling::*)">
            <xsl:value-of select="pg:dup(' ', $listCount)"/>
            <xsl:text>* </xsl:text>
        </xsl:if>
        <xsl:if test="ancestor::db:orderedlist and not(preceding-sibling::*)">
            <xsl:value-of select="pg:dup(' ', $listCount)"/>
            <xsl:text>1. </xsl:text>
        </xsl:if>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:choose>
            <xsl:when test="ancestor::db:blockquote">
                <xsl:text>
</xsl:text>
            </xsl:when>
            <xsl:when test="ancestor::db:listitem and following-sibling::* and
                not(following-sibling::db:itemizedlist or following-sibling::db:orderedlist)">
                <xsl:value-of select="pg:dup('    ', $listCount)"/>
            </xsl:when>
            <xsl:when test="parent::db:glossdef or following-sibling::*">
                <xsl:text>

</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>
</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:template match="db:formalpara" mode="file">
        <xsl:text>**</xsl:text>
        <xsl:value-of select="db:title"/>
        <xsl:text>**: </xsl:text>
        <xsl:apply-templates select="db:para" mode="#current"/>
        <xsl:text>
</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:glossentry/db:glossterm" mode="file">
        <xsl:text>**</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>**:

</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:itemizedlist|db:orderedlist" mode="file">
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>
</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:programlisting" mode="file">
        <xsl:text>{{{
</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>
}}}

</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:glossterm" mode="file">
        <xsl:text>**</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>**</xsl:text>
    </xsl:template>
    <!--
    <xsl:template match="db:footnote" mode="file">
        <xsl:text>[[hottip:</xsl:text>
        <xsl:value-of select="if(@label) then @label else '*'"/>
        <xsl:text>:</xsl:text>
        
        <xsl:apply-templates select="db:para/(*|text())" mode="#current"/>
        <xsl:text>]]</xsl:text>
    </xsl:template>
    -->
    
    <xsl:template match="db:emphasis" mode="file">
        <xsl:text>//</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>//</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:emphasis[@role = 'strong']" mode="file">
        <xsl:text>**</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>**</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:foreignphrase" mode="file">
        <xsl:text>//</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>//</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:filename" mode="file">
        <xsl:text>{{{</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>}}}</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:literal" mode="file">
        <xsl:text>{{{</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>}}}</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:personname" mode="file">
        <xsl:text>**</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>**</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:firstname" mode="file">
        <xsl:if test="preceding-sibling::db:surname">
            <xsl:text> </xsl:text>
        </xsl:if>
        <xsl:apply-templates select="*|text()" mode="#current"/>
    </xsl:template>
    
    <xsl:template match="db:surname" mode="file">
        <xsl:if test="preceding-sibling::db:firstname">
            <xsl:text> </xsl:text>
        </xsl:if>
        <xsl:apply-templates select="*|text()" mode="#current"/>
    </xsl:template>
    
    <xsl:template match="db:quote" mode="file">
        <xsl:text>&quot;</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>&quot;</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:link" mode="file">
        <xsl:text>[[</xsl:text>
        <xsl:value-of select="@xlink:href"/>
        <xsl:text>|</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>]]</xsl:text>
    </xsl:template>
    
    <xsl:template match="pg:pagelink" mode="file">
        <xsl:text>[[</xsl:text>
        <xsl:value-of select="@linkend"/>
        <xsl:text>|</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>]]</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:phrase[@role='title']" mode="file">
        <xsl:text>//**</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>**//</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:citetitle" mode="file">
        <xsl:text>//**</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>**//</xsl:text>
    </xsl:template>
    
    <xsl:template match="db:section" mode="file">
        <xsl:call-template name="MakeSectionTitle"/>
        <xsl:text>

</xsl:text>
        <xsl:apply-templates select="*|text()" mode="#current"/>
    </xsl:template>
    
    <xsl:template name="MakeSectionTitle">
        <xsl:for-each select="ancestor-or-self::db:section">
            <xsl:text>=</xsl:text>
        </xsl:for-each>
        <xsl:variable name="RootSection" select="/*/db:section"/>
        <xsl:variable name="NotSingularRoot" select="$RootSection/following-sibling::* or $RootSection/preceding-sibling::*[name()
            != 'title']"/>
        <xsl:if test="$NotSingularRoot">
            <xsl:text>=</xsl:text>
        </xsl:if>
        <xsl:value-of select="db:title[1]"/>
        <xsl:if test="$NotSingularRoot">
            <xsl:text>=</xsl:text>
        </xsl:if>
        <xsl:for-each select="ancestor-or-self::db:section">
            <xsl:text>=</xsl:text>
        </xsl:for-each>
    </xsl:template>
    
    <xsl:template match="db:blockquote" mode="file">
        <xsl:apply-templates select="*|text()" mode="#current"/>
        <xsl:text>
</xsl:text>
    </xsl:template>
    
    <xsl:template match="text()" mode="file">
        <xsl:variable name="norm" select="normalize-space()"/>
        <xsl:if test="substring($norm, 1, 1) != substring(., 1, 1) and (string-length($norm) > 0)">
            <xsl:text> </xsl:text>
        </xsl:if>
        <xsl:value-of select="$norm"/>
        <xsl:if test="substring($norm, string-length($norm), 1) != substring(., string-length(.), 1) and (string-length($norm) > 0)">
            <xsl:text> </xsl:text>
        </xsl:if>
    </xsl:template>

    <xsl:template match="text()[ancestor::db:programlisting]" mode="file">
        <xsl:value-of select="."/>
    </xsl:template>
    
    <xsl:template match="db:title" mode="file"/>

    <xsl:template match="db:phrase[@role='toc']" mode="file">
        <xsl:apply-templates select="/descendant::db:section" mode="toc"/>
    </xsl:template>
    
    <xsl:template match="db:section" mode="toc">
        <xsl:variable name="title" select="db:title/text()"/>
        <xsl:variable name="link" select="lower-case(replace($title, ' ', '-'))"/>
        <xsl:variable name="starCount" select="count(ancestor-or-self::db:section)"/>
        <xsl:value-of select="pg:dup('*', $starCount)"/>
        <xsl:text> </xsl:text>
        <xsl:text>[[</xsl:text>
        <xsl:value-of select="$basename"/>
        <xsl:text>#!</xsl:text>
        <xsl:value-of select="$link"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$title"/>
        <xsl:text>]]</xsl:text>
        <xsl:text>
</xsl:text>
    </xsl:template>
    
    <xsl:function name="pg:dup">
        <xsl:param name="input"/>
        <xsl:param name="count"/>
        <xsl:sequence select="string-join(for $l in 1 to $count return $input, '')"/>
    </xsl:function>

















</xsl:transform>